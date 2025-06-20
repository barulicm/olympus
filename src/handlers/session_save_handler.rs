use super::Handler;
use crate::app_error::AppError;
use crate::app_state::{AppState, SharedAppState};
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
        let mut new_state: AppState = serde_json::from_value(body).map_err(|e| {
            AppError::new(
                StatusCode::BAD_REQUEST,
                format!("Failed to parse session JSON: {}", e),
            )
        })?;
        new_state.resources_path = app_state.resources_path.clone();
        *app_state = new_state;
        Ok(StatusCode::OK)
    }
}
#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use axum::body::Body;
    use axum::http::{Request, StatusCode};
    use tower::{Service, ServiceExt};

    #[tokio::test]
    async fn export_and_reimport() {
        let app_state = create_new_shared_state();
        let mut app = SessionSaveHandler::register_routes()
            .with_state(app_state)
            .into_service();

        let req = Request::builder()
            .method("GET")
            .uri("/session_backup.json")
            .body(Body::empty())
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);

        let req = Request::builder()
            .method("PUT")
            .uri("/session/import")
            .header(header::CONTENT_TYPE, "application/json")
            .body(response.into_body())
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);
    }
}
