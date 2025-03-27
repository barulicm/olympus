use super::Handler;
use crate::app_state::{SharedAppState, Team};
use axum::{
    Router,
    extract::{Json, State},
    http::{StatusCode, header},
    response::IntoResponse,
    routing::{get, put},
};
use serde_json::Value;

pub struct ScoresHandler;

impl Handler for ScoresHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new()
            .route("/scores/export.csv", get(Self::handle_export_scores))
            .route("/scores/export_gp.csv", get(Self::handle_export_gp_scores))
            .route("/scores/submit", put(Self::handle_submit))
            .route("/scores/rerank", put(Self::handle_rerank))
    }
}

impl ScoresHandler {
    async fn handle_export_scores(State(app_state): State<SharedAppState>) -> impl IntoResponse {
        let app_state = app_state.lock().unwrap();
        let mut teams = app_state.teams.clone();
        teams.sort_by_key(|t| t.number.clone());
        let score_count = teams.iter().map(|t| t.scores.len()).max().unwrap();
        let mut csv_writer = csv::Writer::from_writer(vec![]);
        let mut headers = vec![
            String::from("rank"),
            String::from("team number"),
            String::from("team name"),
        ];
        for i in 1..=score_count {
            headers.push(format!("match {}", i).as_str().to_string());
        }
        headers.push(String::from("final score"));
        csv_writer.write_record(headers).unwrap();
        for team in teams {
            let mut record = vec![team.rank.to_string(), team.number, team.name];
            let mut score_strings = team
                .scores
                .iter()
                .map(|s| s.to_string())
                .collect::<Vec<_>>();
            record.append(&mut score_strings);
            record.push(team.display_score.to_string());
            csv_writer.write_record(record).unwrap();
        }
        let response_body = String::from_utf8(csv_writer.into_inner().unwrap()).unwrap();
        (
            StatusCode::OK,
            [(header::CONTENT_TYPE, "text/csv")],
            response_body,
        )
    }

    async fn handle_export_gp_scores(State(app_state): State<SharedAppState>) -> impl IntoResponse {
        let app_state = app_state.lock().unwrap();
        let mut teams = app_state.teams.clone();
        teams.sort_by_key(|t| t.number.clone());
        let score_count = teams.iter().map(|t| t.scores.len()).max().unwrap();
        let mut csv_writer = csv::Writer::from_writer(vec![]);
        let mut headers = vec![String::from("team number"), String::from("team name")];
        for i in 1..=score_count {
            headers.push(format!("match {}", i).as_str().to_string());
        }
        csv_writer.write_record(headers).unwrap();
        for team in teams {
            let mut record = vec![team.number, team.name];
            let mut score_strings = team
                .gp_scores
                .iter()
                .map(|s| s.to_string())
                .collect::<Vec<_>>();
            record.append(&mut score_strings);
            csv_writer.write_record(record).unwrap();
        }
        let response_body = String::from_utf8(csv_writer.into_inner().unwrap()).unwrap();
        (
            StatusCode::OK,
            [(header::CONTENT_TYPE, "text/csv")],
            response_body,
        )
    }

    async fn handle_submit(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> impl IntoResponse {
        let team_number = body.get("teamNumber");
        if team_number.is_none() {
            return (
                StatusCode::BAD_REQUEST,
                "Missing required field: teamNumber",
            );
        }
        let team_number = team_number.unwrap().as_str().unwrap();
        let mut app_state = app_state.lock().unwrap();
        let team = app_state.teams.iter_mut().find(|t| t.number == team_number);
        if team.is_none() {
            return (
                StatusCode::NOT_FOUND,
                "Score submission failed. Nonexistent team number.",
            );
        }
        let team = team.unwrap();
        team.scores.push(
            body.get("score")
                .unwrap()
                .as_number()
                .unwrap()
                .as_i64()
                .unwrap(),
        );
        team.gp_scores.push(
            body.get("gpScore")
                .unwrap()
                .as_number()
                .unwrap()
                .as_i64()
                .unwrap(),
        );
        Self::update_ranks(&mut app_state.teams);
        (StatusCode::OK, "")
    }

    async fn handle_rerank(State(app_state): State<SharedAppState>) -> impl IntoResponse {
        let mut app_state = app_state.lock().unwrap();
        Self::update_ranks(&mut app_state.teams);
        StatusCode::OK
    }

    fn update_ranks(teams: &mut Vec<Team>) {
        for team in teams.iter_mut() {
            if team.scores.is_empty() {
                team.display_score = 0;
            } else {
                team.display_score = *team.scores.iter().max().unwrap();
            }
        }
        teams.sort_by_key(|t| t.display_score);
        let mut rank = 1;
        teams[0].rank = rank;
        for i in 1..teams.len() {
            if teams[i].display_score > teams[i - 1].display_score {
                rank += 1;
            }
            teams[i].rank = rank;
        }
    }
}
