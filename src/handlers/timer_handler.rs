use super::Handler;
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
    async fn handle_get(Extension(handler): Extension<SharedTimerHandler>) -> impl IntoResponse {
        let handler = handler.lock().unwrap();
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
        (
            StatusCode::OK,
            Json::from(json!({"time_remaining": seconds_left})),
        )
    }

    async fn handle_start_timer(
        Extension(handler): Extension<SharedTimerHandler>,
    ) -> impl IntoResponse {
        let mut handler = handler.lock().unwrap();
        if handler.timer_running {
            return (
                StatusCode::INTERNAL_SERVER_ERROR,
                "Timer is already running",
            );
        }
        handler.timer_running = true;
        handler.timer_start_time = Instant::now();
        (StatusCode::OK, "")
    }

    async fn handle_stop_timer(
        Extension(handler): Extension<SharedTimerHandler>,
    ) -> impl IntoResponse {
        let mut handler = handler.lock().unwrap();
        handler.timer_running = false;
        (StatusCode::OK, "")
    }
}
