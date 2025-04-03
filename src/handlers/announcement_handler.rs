use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use axum::{
    Router,
    extract::{Extension, Json},
    http::StatusCode,
    response::IntoResponse,
    routing::{get, put},
};
use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::sync::{Arc, Mutex};

#[derive(Clone, Serialize, Deserialize)]
pub struct AnnouncementHandler {
    content: String,
    visible: bool,
}

type SharedAnnouncementHandler = Arc<Mutex<AnnouncementHandler>>;

impl Handler for AnnouncementHandler {
    fn register_routes() -> Router<SharedAppState> {
        let handler = Arc::new(Mutex::new(AnnouncementHandler {
            content: String::from(""),
            visible: false,
        }));
        Router::new()
            .route("/announcement", get(Self::handle_get))
            .route("/announcement", put(Self::handle_put))
            .layer(Extension(handler))
    }
}

impl AnnouncementHandler {
    async fn handle_get(
        Extension(handler): Extension<SharedAnnouncementHandler>,
    ) -> Result<impl IntoResponse, AppError> {
        let handler = handler.lock()?;
        Ok(axum::Json(handler.clone()))
    }

    async fn handle_put(
        Extension(handler): Extension<SharedAnnouncementHandler>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut handler = handler.lock()?;
        *handler = serde_json::from_value(body).map_err(|e| {
            AppError::new(
                StatusCode::BAD_REQUEST,
                format!("Failed to parse request body: {}", &e),
            )
        })?;
        Ok(StatusCode::OK)
    }
}
