use super::Handler;
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
    async fn export_session(State(app_state): State<SharedAppState>) -> impl IntoResponse {
        let app_state = app_state.lock().unwrap();
        let session_json = serde_json::to_string(&*app_state).unwrap();
        let response = axum::http::Response::builder()
            .header(header::CONTENT_TYPE, "application/x-download-me")
            .body(session_json)
            .unwrap();
        response
    }

    async fn import_session(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> impl IntoResponse {
        let mut app_state = app_state.lock().unwrap();
        let parse_result = serde_json::from_value(body);
        match parse_result {
            Ok(new_state) => {
                *app_state = new_state;
                (StatusCode::OK, String::new())
            }
            Err(e) => (
                StatusCode::BAD_REQUEST,
                format!("Error importing session: {:?}", e),
            ),
        }
    }
}
