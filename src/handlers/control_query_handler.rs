use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use axum::{
    Router,
    extract::State,
    http::{StatusCode, header::HeaderMap},
    response::IntoResponse,
    routing::get,
};

pub struct ControlQueryHandler;

impl Handler for ControlQueryHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new().route("/controlQuery", get(Self::handle_get))
    }
}

impl ControlQueryHandler {
    async fn handle_get(
        State(app_state): State<SharedAppState>,
        headers: HeaderMap,
    ) -> Result<impl IntoResponse, AppError> {
        let query = headers
            .get("query")
            .ok_or((StatusCode::BAD_REQUEST, "Missing query header"))?
            .to_str()
            .map_err(|_| {
                (
                    StatusCode::BAD_REQUEST,
                    "'query' header is not an ASCII string",
                )
            })?;
        let app_state = app_state.lock()?;
        match query {
            "hasTeams" => Ok((
                StatusCode::OK,
                if !app_state.teams.is_empty() {
                    String::from("true")
                } else {
                    String::from("false")
                },
            )),
            "hasGame" => Ok((
                StatusCode::OK,
                if app_state.game_description.is_some() {
                    String::from("true")
                } else {
                    String::from("false")
                },
            )),
            "version" => Ok((StatusCode::OK, String::from("0000.0-0000000"))),
            _ => Err((StatusCode::BAD_REQUEST, "Unrecognized query").into()),
        }
    }
}
