use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use axum::{
    Json, Router,
    extract::Extension,
    http::StatusCode,
    response::IntoResponse,
    routing::{get, put},
};
use serde_json::json;
use std::sync::{Arc, Mutex};
use std::time::Instant;

#[derive(Clone)]
pub struct TimerHandler {
    match_length: u64,
    timer_start_time: Instant,
    timer_running: bool,
}

type SharedTimerHandler = Arc<Mutex<TimerHandler>>;

impl Handler for TimerHandler {
    fn register_routes() -> Router<SharedAppState> {
        let handler = Arc::new(Mutex::new(TimerHandler {
            match_length: 150,
            timer_start_time: Instant::now(),
            timer_running: false,
        }));
        Router::new()
            .route("/timer", get(Self::handle_get))
            .route("/timer/start", put(Self::handle_start_timer))
            .route("/timer/stop", put(Self::handle_stop_timer))
            .layer(Extension(handler))
    }
}

impl TimerHandler {
    async fn handle_get(
        Extension(handler): Extension<SharedTimerHandler>,
    ) -> Result<impl IntoResponse, AppError> {
        let handler = handler.lock()?;
        let seconds_left = if handler.timer_running {
            let seconds_elapsed = handler.timer_start_time.elapsed().as_secs();
            if seconds_elapsed < handler.match_length {
                handler.match_length - seconds_elapsed
            } else {
                0
            }
        } else {
            handler.match_length
        };
        Ok((
            StatusCode::OK,
            Json::from(json!({"time_remaining": seconds_left})),
        ))
    }

    async fn handle_start_timer(
        Extension(handler): Extension<SharedTimerHandler>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut handler = handler.lock()?;
        if handler.timer_running {
            return Err((
                StatusCode::INTERNAL_SERVER_ERROR,
                "Timer is already running",
            )
                .into());
        }
        handler.timer_running = true;
        handler.timer_start_time = Instant::now();
        Ok(StatusCode::OK)
    }

    async fn handle_stop_timer(
        Extension(handler): Extension<SharedTimerHandler>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut handler = handler.lock()?;
        handler.timer_running = false;
        Ok(StatusCode::OK)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use axum::body::Body;
    use axum::http::{Request, StatusCode};
    use http_body_util::BodyExt;
    use serde_json::{Value, json};
    use tower::{Service, ServiceExt};

    #[tokio::test]
    async fn test_get() {
        let app_state = create_new_shared_state();
        let app = TimerHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/timer")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(body, json!({ "time_remaining": 150 }));
    }

    #[tokio::test]
    async fn test_start_timer() {
        let app_state = create_new_shared_state();
        let app = TimerHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("PUT")
            .uri("/timer/start")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
    }

    #[tokio::test]
    async fn test_start_timer_arleady_running() {
        let app_state = create_new_shared_state();
        let mut app = TimerHandler::register_routes()
            .with_state(app_state)
            .into_service();

        let req = Request::builder()
            .method("PUT")
            .uri("/timer/start")
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
            .uri("/timer/start")
            .body(Body::empty())
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::INTERNAL_SERVER_ERROR);
    }

    #[tokio::test]
    async fn test_sop_timer() {
        let app_state = create_new_shared_state();
        let app = TimerHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("PUT")
            .uri("/timer/stop")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
    }
}
