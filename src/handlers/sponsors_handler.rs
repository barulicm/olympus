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

#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use axum::body::Body;
    use axum::http::{Request, StatusCode, header};
    use http_body_util::BodyExt;
    use serde_json::{Value, json};
    use tower::ServiceExt;

    #[tokio::test]
    async fn defaults_to_no_sponsors() {
        let app_state = create_new_shared_state();
        let app = SponsorsHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/sponsors")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(body, json!({ "sponsors": Vec::<String>::new() }));
    }

    #[tokio::test]
    async fn add_sponsor() {
        let app_state = create_new_shared_state();
        let app = SponsorsHandler::register_routes().with_state(app_state.clone());

        let req = Request::builder()
            .method("PUT")
            .uri("/sponsors/add")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"image_data": "data:image/00000000"}"#))
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        assert_eq!(
            app_state.lock().unwrap().sponsor_logos,
            vec![String::from("data:image/00000000")]
        );
    }

    #[tokio::test]
    async fn delete_sponsor() {
        let app_state = create_new_shared_state();
        app_state.lock().unwrap().sponsor_logos = vec![
            String::from("data:image/00000000"),
            String::from("data:image/11111111"),
        ];
        let app = SponsorsHandler::register_routes().with_state(app_state.clone());

        let req = Request::builder()
            .method("PUT")
            .uri("/sponsors/delete")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"index": 0}"#))
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        assert_eq!(
            app_state.lock().unwrap().sponsor_logos,
            vec![String::from("data:image/11111111")]
        );
    }

    #[tokio::test]
    async fn delete_all_sponsors() {
        let app_state = create_new_shared_state();
        app_state.lock().unwrap().sponsor_logos = vec![
            String::from("data:image/00000000"),
            String::from("data:image/11111111"),
        ];
        let app = SponsorsHandler::register_routes().with_state(app_state.clone());

        let req = Request::builder()
            .method("PUT")
            .uri("/sponsors/delete_all")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        assert!(app_state.lock().unwrap().sponsor_logos.is_empty());
    }
}
