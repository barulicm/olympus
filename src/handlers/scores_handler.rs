use super::Handler;
use crate::app_error::AppError;
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
    async fn handle_export_scores(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let mut teams = app_state.teams.clone();
        teams.sort_by_key(|t| t.number.clone());
        let score_count = teams.iter().map(|t| t.scores.len()).max().unwrap_or(0);
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
        csv_writer.write_record(headers).map_err(|e| {
            AppError::new(
                StatusCode::INTERNAL_SERVER_ERROR,
                format!("Failed to add csv record: {}", e),
            )
        })?;
        for team in teams {
            let mut record = vec![team.rank.to_string(), team.number, team.name];
            let mut score_strings = team
                .scores
                .iter()
                .map(|s| s.to_string())
                .collect::<Vec<_>>();
            record.append(&mut score_strings);
            record.push(team.display_score.to_string());
            csv_writer.write_record(record).map_err(|e| {
                AppError::new(
                    StatusCode::INTERNAL_SERVER_ERROR,
                    format!("Failed to add csv record: {}", e),
                )
            })?;
        }
        let csv_inner_writer = csv_writer.into_inner().map_err(|e| {
            AppError::new(
                StatusCode::INTERNAL_SERVER_ERROR,
                format!("Failed to finalize CSV writer: {}", e),
            )
        })?;
        let response_body = String::from_utf8(csv_inner_writer).map_err(|e| {
            AppError::new(
                StatusCode::INTERNAL_SERVER_ERROR,
                format!("Failed to convert CSV to UTF-8: {}", e),
            )
        })?;
        Ok((
            StatusCode::OK,
            [(header::CONTENT_TYPE, "text/csv")],
            response_body,
        ))
    }

    async fn handle_export_gp_scores(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let mut teams = app_state.teams.clone();
        teams.sort_by_key(|t| t.number.clone());
        let score_count = teams.iter().map(|t| t.scores.len()).max().unwrap_or(0);
        let mut csv_writer = csv::Writer::from_writer(vec![]);
        let mut headers = vec![String::from("team number"), String::from("team name")];
        for i in 1..=score_count {
            headers.push(format!("match {}", i).as_str().to_string());
        }
        csv_writer.write_record(headers).map_err(|e| {
            AppError::new(
                StatusCode::INTERNAL_SERVER_ERROR,
                format!("Failed to add csv record: {}", e),
            )
        })?;
        for team in teams {
            let mut record = vec![team.number, team.name];
            let mut score_strings = team
                .gp_scores
                .iter()
                .map(|s| s.to_string())
                .collect::<Vec<_>>();
            record.append(&mut score_strings);
            csv_writer.write_record(record).map_err(|e| {
                AppError::new(
                    StatusCode::INTERNAL_SERVER_ERROR,
                    format!("Failed to add csv record: {}", e),
                )
            })?;
        }
        let csv_inner_writer = csv_writer.into_inner().map_err(|e| {
            AppError::new(
                StatusCode::INTERNAL_SERVER_ERROR,
                format!("Failed to finalize CSV writer: {}", e),
            )
        })?;
        let response_body = String::from_utf8(csv_inner_writer).map_err(|e| {
            AppError::new(
                StatusCode::INTERNAL_SERVER_ERROR,
                format!("Failed to convert CSV to UTF-8: {}", e),
            )
        })?;
        Ok((
            StatusCode::OK,
            [(header::CONTENT_TYPE, "text/csv")],
            response_body,
        ))
    }

    async fn handle_submit(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let team_number = body
            .get("teamNumber")
            .and_then(|s| s.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid team number"),
            ))?;
        let mut app_state = app_state.lock()?;
        let team = app_state
            .teams
            .iter_mut()
            .find(|t| t.number == team_number)
            .ok_or(AppError::new(
                StatusCode::NOT_FOUND,
                format!(
                    "Score submission failed. Nonexistent team number: {}",
                    team_number
                ),
            ))?;
        let new_score = body
            .get("score")
            .and_then(|s| s.as_number())
            .and_then(|n| n.as_i64())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid score value"),
            ))?;
        team.scores.push(new_score);
        let new_gp_score = body
            .get("gpScore")
            .and_then(|s| s.as_number())
            .and_then(|n| n.as_i64())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid gpScore value"),
            ))?;
        team.gp_scores.push(new_gp_score);
        Self::update_ranks(&mut app_state.teams);
        Ok(StatusCode::OK)
    }

    async fn handle_rerank(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut app_state = app_state.lock()?;
        Self::update_ranks(&mut app_state.teams);
        Ok(StatusCode::OK)
    }

    fn update_ranks(teams: &mut [Team]) {
        for team in teams.iter_mut() {
            team.display_score = *team.scores.iter().max().unwrap_or(&0);
        }
        teams.sort_by_key(|t| t.display_score);
        teams.reverse();
        let mut rank = 1;
        teams[0].rank = rank;
        for i in 1..teams.len() {
            if teams[i].display_score < teams[i - 1].display_score {
                rank += 1;
            }
            teams[i].rank = rank;
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use axum::body::Body;
    use axum::http::{Request, StatusCode, header};
    use http_body_util::BodyExt;
    use tower::ServiceExt;

    #[tokio::test]
    async fn export_scores() {
        let app_state = create_new_shared_state();
        let team = Team {
            number: String::from("1234"),
            name: String::from("Test Team"),
            scores: vec![10, 0, 25],
            gp_scores: vec![2, 3, 4],
            display_score: 25,
            rank: 1,
        };
        app_state.lock().unwrap().teams.push(team);
        let app = ScoresHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/scores/export.csv")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let expected_body = b"rank,team number,team name,match 1,match 2,match 3,final score\n1,1234,Test Team,10,0,25,25\n";
        assert_eq!(&body[..], expected_body);
    }

    #[tokio::test]
    async fn export_gp_scores() {
        let app_state = create_new_shared_state();
        let team = Team {
            number: String::from("1234"),
            name: String::from("Test Team"),
            scores: vec![10, 0, 25],
            gp_scores: vec![2, 3, 4],
            display_score: 25,
            rank: 1,
        };
        app_state.lock().unwrap().teams.push(team);
        let app = ScoresHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/scores/export_gp.csv")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let expected_body =
            b"team number,team name,match 1,match 2,match 3\n1234,Test Team,2,3,4\n";
        assert_eq!(&body[..], expected_body);
    }

    #[tokio::test]
    async fn submit_score() {
        let app_state = create_new_shared_state();
        let team = Team::new(String::from("1234"), String::from("Test Team"));
        app_state.lock().unwrap().teams.push(team);
        let app = ScoresHandler::register_routes().with_state(app_state.clone());

        let req = Request::builder()
            .method("PUT")
            .uri("/scores/submit")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(
                "\
                {\"teamNumber\": \"1234\", \"score\": 10, \"gpScore\": 2}",
            ))
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let team = &app_state.lock().unwrap().teams[0];
        assert_eq!(team.scores, vec![10]);
        assert_eq!(team.gp_scores, vec![2]);
    }

    #[tokio::test]
    async fn rerank() {
        let app_state = create_new_shared_state();
        app_state.lock().unwrap().teams = vec![
            Team {
                number: String::from("1234"),
                name: String::from("Test Team A"),
                scores: vec![10, 20],
                gp_scores: vec![2, 2],
                display_score: 0,
                rank: 0,
            },
            Team {
                number: String::from("5678"),
                name: String::from("Test Team B"),
                scores: vec![11, 12],
                gp_scores: vec![2, 2],
                display_score: 0,
                rank: 0,
            },
            Team {
                number: String::from("9012"),
                name: String::from("Test Team C"),
                scores: vec![50, 0],
                gp_scores: vec![2, 2],
                display_score: 0,
                rank: 0,
            },
        ];
        let app = ScoresHandler::register_routes().with_state(app_state.clone());

        let req = Request::builder()
            .method("PUT")
            .uri("/scores/rerank")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let teams = &app_state.lock().unwrap().teams;
        assert_eq!(
            teams.iter().map(|t| t.rank).collect::<Vec<_>>(),
            vec![1, 2, 3]
        );
        assert_eq!(
            teams.iter().map(|t| t.display_score).collect::<Vec<_>>(),
            vec![50, 20, 12]
        );
        assert_eq!(
            teams.iter().map(|t| t.number.clone()).collect::<Vec<_>>(),
            vec![
                String::from("9012"),
                String::from("1234"),
                String::from("5678")
            ]
        );
    }
}
