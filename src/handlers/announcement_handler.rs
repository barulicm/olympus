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

#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use axum::body::Body;
    use axum::http::{Request, StatusCode, header};
    use http_body_util::BodyExt;
    use serde_json::{Value, json};
    use tower::{Service, ServiceExt};

    #[tokio::test]
    async fn defaults_to_no_announcement() {
        let app_state = create_new_shared_state();
        let app = AnnouncementHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/announcement")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(body, json!({ "content": "", "visible": false }));
    }

    #[tokio::test]
    async fn setting_announcement_works() {
        let app_state = create_new_shared_state();
        let mut app = AnnouncementHandler::register_routes()
            .with_state(app_state)
            .into_service();

        let req = Request::builder()
            .method("PUT")
            .uri("/announcement")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(
                r#"{"content": "Test announcement", "visible": true}"#,
            ))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);

        let req = Request::builder()
            .method("GET")
            .uri("/announcement")
            .body(Body::empty())
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(
            body,
            json!({ "content": "Test announcement", "visible": true })
        );

        let req = Request::builder()
            .method("PUT")
            .uri("/announcement")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"content": "", "visible": false}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);

        let req = Request::builder()
            .method("GET")
            .uri("/announcement")
            .body(Body::empty())
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(body, json!({ "content": "", "visible": false }));
    }
}
