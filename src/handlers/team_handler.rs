use super::Handler;
use crate::app_state::{SharedAppState, Team};
use axum::{
    Router,
    extract::{Json, Path, State},
    http::{StatusCode, header},
    response::IntoResponse,
    routing::{get, put},
};
use serde_json::Value;

pub struct TeamHandler;

impl Handler for TeamHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new()
            .route("/team/add", put(Self::handle_add_team))
            .route("/team/remove", put(Self::handle_remove_team))
            .route("/team/edit", put(Self::handle_edit_team))
            .route("/team/{team_number}", get(Self::handle_get))
    }
}

impl TeamHandler {
    async fn handle_get(
        State(app_state): State<SharedAppState>,
        Path(team_number): Path<String>,
    ) -> impl IntoResponse {
        let app_state = app_state.lock().unwrap();
        if team_number == "all" {
            (
                StatusCode::OK,
                [(header::CONTENT_TYPE, "application/json")],
                serde_json::to_string(&app_state.teams).unwrap(),
            )
        } else {
            let team = app_state.teams.iter().find(|t| t.number == team_number);
            match team {
                Some(t) => (
                    StatusCode::OK,
                    [(header::CONTENT_TYPE, "application/json")],
                    serde_json::to_string(&t).unwrap(),
                ),
                None => (
                    StatusCode::BAD_REQUEST,
                    [(header::CONTENT_TYPE, "text/plain")],
                    String::from("No team with requested number"),
                ),
            }
        }
    }

    async fn handle_add_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> impl IntoResponse {
        let team_name = body.get("name").unwrap().as_str().unwrap().to_string();
        let team_number = body.get("number").unwrap().as_str().unwrap().to_string();
        let new_team = Team::new(team_number, team_name);
        if new_team.number.is_empty() {
            return (
                StatusCode::BAD_REQUEST,
                String::from("Team number must not be empty"),
            );
        }
        let mut app_state = app_state.lock().unwrap();
        let old_team = app_state.teams.iter().find(|t| t.number == new_team.number);
        if old_team.is_some() {
            return (
                StatusCode::BAD_REQUEST,
                String::from("Another team is already using the new team number."),
            );
        }
        app_state.teams.push(new_team);
        (StatusCode::OK, String::new())
    }

    async fn handle_remove_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> impl IntoResponse {
        let team_number = body.get("number");
        if team_number.is_none() {
            return (
                StatusCode::BAD_REQUEST,
                String::from("Invalid request body"),
            );
        }
        let team_number = team_number.unwrap().as_str().unwrap();
        let mut app_state = app_state.lock().unwrap();
        let found_team_position = app_state.teams.iter().position(|t| t.number == team_number);
        if let Some(position) = found_team_position {
            app_state.teams.remove(position);
            (StatusCode::OK, String::from("Remove team successful."))
        } else {
            (StatusCode::NOT_FOUND, String::from("No such team."))
        }
    }

    async fn handle_edit_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> impl IntoResponse {
        let old_team_number = body.get("oldTeamNumber").unwrap().as_str().unwrap();
        let new_team_number = body.get("newTeamNumber").unwrap().as_str().unwrap();
        if new_team_number.is_empty() {
            return (StatusCode::BAD_REQUEST, "Team number must not be empty.");
        }
        let mut app_state = app_state.lock().unwrap();
        if new_team_number != old_team_number {
            if app_state.teams.iter().any(|t| t.number == new_team_number) {
                return (
                    StatusCode::BAD_REQUEST,
                    "Another team is already using the new team number.",
                );
            }
        }
        let team = app_state
            .teams
            .iter_mut()
            .find(|t| t.number == old_team_number);
        if let Some(team) = team {
            team.number = new_team_number.to_string();
            team.name = body
                .get("newTeamName")
                .unwrap()
                .as_str()
                .unwrap()
                .to_string();
            team.scores =
                serde_json::from_value::<Vec<i64>>(body.get("newScores").unwrap().clone()).unwrap();
            team.gp_scores =
                serde_json::from_value::<Vec<i64>>(body.get("newGPScores").unwrap().clone())
                    .unwrap();
            (StatusCode::OK, "Team edits saved.")
        } else {
            (StatusCode::NOT_FOUND, "No such team.")
        }
    }
}
