use super::Handler;
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
    async fn handle_get(State(app_state): State<SharedAppState>) -> impl IntoResponse {
        let app_state = app_state.lock().unwrap();
        axum::Json(json!({"sponsors": app_state.sponsor_logos.clone()}))
    }

    async fn handle_add_sponsor(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> impl IntoResponse {
        let mut app_state = app_state.lock().unwrap();
        let image_data = body
            .get("image_data")
            .unwrap()
            .as_str()
            .unwrap()
            .to_string();
        if !image_data.starts_with("data:image/") {
            return (
                StatusCode::BAD_REQUEST,
                "Failed to add sponsor: File not recognized as an image.",
            );
        }
        app_state.sponsor_logos.push(image_data);
        (StatusCode::OK, "")
    }

    async fn handle_delete_sponsor(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> impl IntoResponse {
        let mut app_state = app_state.lock().unwrap();
        let index = body
            .get("index")
            .unwrap()
            .as_i64()
            .unwrap()
            .try_into()
            .unwrap();
        if index > app_state.sponsor_logos.len() {
            return (
                StatusCode::BAD_REQUEST,
                "Failed to add sponsor: Index out of range.",
            );
        }
        app_state.sponsor_logos.remove(index);
        (StatusCode::OK, "")
    }

    async fn handle_delete_all_sponsors(
        State(app_state): State<SharedAppState>,
    ) -> impl IntoResponse {
        let mut app_state = app_state.lock().unwrap();
        app_state.sponsor_logos.clear();
        StatusCode::OK
    }
}
