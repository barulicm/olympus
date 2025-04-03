use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use axum::{
    Router,
    extract::{Json, State},
    http::StatusCode,
    response::IntoResponse,
    routing::{get, put},
};
use serde_json::{Value, json};

pub struct SponsorsHandler;

impl Handler for SponsorsHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new()
            .route("/sponsors", get(Self::handle_get))
            .route("/sponsors/add", put(Self::handle_add_sponsor))
            .route("/sponsors/delete", put(Self::handle_delete_sponsor))
            .route(
                "/sponsors/delete_all",
                put(Self::handle_delete_all_sponsors),
            )
    }
}

impl SponsorsHandler {
    async fn handle_get(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        Ok(axum::Json(
            json!({"sponsors": app_state.sponsor_logos.clone()}),
        ))
    }

    async fn handle_add_sponsor(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut app_state = app_state.lock()?;
        let image_data = body
            .get("image_data")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid image_data"),
            ))?
            .to_string();
        if !image_data.starts_with("data:image/") {
            return Err((
                StatusCode::BAD_REQUEST,
                "Failed to add sponsor: File not recognized as an image.",
            )
                .into());
        }
        app_state.sponsor_logos.push(image_data);
        Ok(StatusCode::OK)
    }

    async fn handle_delete_sponsor(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut app_state = app_state.lock()?;
        let index = body
            .get("index")
            .and_then(|v| v.as_i64())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid index"),
            ))?
            .try_into()
            .map_err(|e| {
                AppError::new(StatusCode::BAD_REQUEST, format!("Bad index value: {}", e))
            })?;
        if index > app_state.sponsor_logos.len() {
            return Err((
                StatusCode::BAD_REQUEST,
                "Failed to add sponsor: Index out of range.",
            )
                .into());
        }
        app_state.sponsor_logos.remove(index);
        Ok(StatusCode::OK)
    }

    async fn handle_delete_all_sponsors(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut app_state = app_state.lock()?;
        app_state.sponsor_logos.clear();
        Ok(StatusCode::OK)
    }
}
