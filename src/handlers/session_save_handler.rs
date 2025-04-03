use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use axum::{
    Router,
    extract::{Json, State},
    http::{StatusCode, header},
    response::IntoResponse,
    routing::{get, put},
};
use serde_json::Value;

pub struct SessionSaveHandler;

impl Handler for SessionSaveHandler {
    fn register_routes() -> axum::Router<crate::app_state::SharedAppState> {
        Router::new()
            .route("/session_backup.json", get(Self::export_session))
            .route("/session/import", put(Self::import_session))
    }
}

impl SessionSaveHandler {
    async fn export_session(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let session_json = serde_json::to_string(&*app_state)?;
        axum::http::Response::builder()
            .header(header::CONTENT_TYPE, "application/x-download-me")
            .body(session_json)
            .map_err(|e| {
                AppError::new(
                    StatusCode::INTERNAL_SERVER_ERROR,
                    format!("Failed to create response: {}", e),
                )
            })
    }

    async fn import_session(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut app_state = app_state.lock()?;
        let new_state = serde_json::from_value(body).map_err(|e| {
            AppError::new(
                StatusCode::BAD_REQUEST,
                format!("Failed to parse session JSON: {}", e),
            )
        })?;
        *app_state = new_state;
        Ok(StatusCode::OK)
    }
}
