use super::Handler;
use crate::app_error::AppError;
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
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        if team_number == "all" {
            let response_body = serde_json::to_string(&app_state.teams).map_err(|e| {
                AppError::new(
                    StatusCode::INTERNAL_SERVER_ERROR,
                    format!("Failed to serialize teams: {}", e),
                )
            })?;
            Ok((
                StatusCode::OK,
                [(header::CONTENT_TYPE, "application/json")],
                response_body,
            ))
        } else {
            let team = app_state
                .teams
                .iter()
                .find(|t| t.number == team_number)
                .ok_or(AppError::new(
                    StatusCode::BAD_REQUEST,
                    format!("No team with number '{}'", team_number),
                ))?;
            let response_body = serde_json::to_string(&team).map_err(|e| {
                AppError::new(
                    StatusCode::INTERNAL_SERVER_ERROR,
                    format!("Failed to serialize team: {}", e),
                )
            })?;
            Ok((
                StatusCode::OK,
                [(header::CONTENT_TYPE, "application/json")],
                response_body,
            ))
        }
    }

    async fn handle_add_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let team_name = body
            .get("name")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid team name."),
            ))?
            .to_string();
        let team_number = body
            .get("number")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid team number."),
            ))?
            .to_string();
        let new_team = Team::new(team_number, team_name);
        if new_team.number.is_empty() {
            return Err((
                StatusCode::BAD_REQUEST,
                String::from("Team number must not be empty"),
            )
                .into());
        }
        let mut app_state = app_state.lock()?;
        let old_team = app_state.teams.iter().find(|t| t.number == new_team.number);
        if old_team.is_some() {
            return Err((
                StatusCode::BAD_REQUEST,
                "Another team is already using the new team number.",
            )
                .into());
        }
        app_state.teams.push(new_team);
        Ok(StatusCode::OK)
    }

    async fn handle_remove_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let team_number = body
            .get("number")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid team number."),
            ))?;
        let mut app_state = app_state.lock()?;
        let found_team_position = app_state
            .teams
            .iter()
            .position(|t| t.number == team_number)
            .ok_or(AppError::new(
                StatusCode::NOT_FOUND,
                format!("No team with number '{}'", team_number),
            ))?;
        app_state.teams.remove(found_team_position);
        Ok((StatusCode::OK, "Remove team successful."))
    }

    async fn handle_edit_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let old_team_number =
            body.get("oldTeamNumber")
                .and_then(|v| v.as_str())
                .ok_or(AppError::new(
                    StatusCode::BAD_REQUEST,
                    String::from("Missing or invalid old team number."),
                ))?;
        let new_team_number =
            body.get("newTeamNumber")
                .and_then(|v| v.as_str())
                .ok_or(AppError::new(
                    StatusCode::BAD_REQUEST,
                    String::from("Missing or invalid new team number."),
                ))?;
        if new_team_number.is_empty() {
            return Err((StatusCode::BAD_REQUEST, "Team number must not be empty.").into());
        }
        let mut app_state = app_state.lock()?;
        if new_team_number != old_team_number
            && app_state.teams.iter().any(|t| t.number == new_team_number)
        {
            return Err((
                StatusCode::BAD_REQUEST,
                "Another team is already using the new team number.",
            )
                .into());
        }
        let team = app_state
            .teams
            .iter_mut()
            .find(|t| t.number == old_team_number)
            .ok_or(AppError::new(
                StatusCode::NOT_FOUND,
                format!("No team with number '{}'", old_team_number),
            ))?;
        let new_team_name = body
            .get("newTeamNumber")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid new team number."),
            ))?
            .to_string();
        let new_scores = body.get("newScores").ok_or(AppError::new(
            StatusCode::BAD_REQUEST,
            String::from("Missing new scores."),
        ))?;
        let new_scores = serde_json::from_value(new_scores.clone())?;
        let new_gp_scores = body.get("newGPScores").ok_or(AppError::new(
            StatusCode::BAD_REQUEST,
            String::from("Missing new GP scores."),
        ))?;
        let new_gp_scores = serde_json::from_value(new_gp_scores.clone())?;

        team.number = new_team_number.to_string();
        team.name = new_team_name;
        team.scores = new_scores;
        team.gp_scores = new_gp_scores;
        Ok((StatusCode::OK, "Team edits saved."))
    }
}
