use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use axum::{
    Json, Router,
    extract::{
        Extension,
        ws::{Message, Utf8Bytes, WebSocket, WebSocketUpgrade},
    },
    http::StatusCode,
    response::IntoResponse,
    routing::{any, get, put},
};
use serde_json::json;
use std::sync::{Arc, Mutex, mpsc};
use std::time::Instant;
use tokio::time;

#[derive(Clone)]
struct TimerState {
    match_length: u64,
    timer_start_time: Instant,
    timer_running: bool,
}

impl TimerState {
    fn default() -> Self {
        TimerState {
            match_length: 150,
            timer_start_time: Instant::now(),
            timer_running: false,
        }
    }
}

#[derive(Clone)]
pub struct TimerHandler {
    timer_state: TimerState,
    senders: Vec<mpsc::Sender<TimerState>>,
}

type SharedTimerHandler = Arc<Mutex<TimerHandler>>;

impl Handler for TimerHandler {
    fn register_routes() -> Router<SharedAppState> {
        let handler = Arc::new(Mutex::new(TimerHandler {
            timer_state: TimerState::default(),
            senders: vec![],
        }));
        Router::new()
            .route("/timer", get(Self::handle_get))
            .route("/timer/start", put(Self::handle_start_timer))
            .route("/timer/stop", put(Self::handle_stop_timer))
            .route("/timer/ws", any(Self::handle_websocket_upgrade))
            .layer(Extension(handler))
    }
}

fn get_seconds_remaining(timer_state: &TimerState) -> u64 {
    if timer_state.timer_running {
        let seconds_elapsed = timer_state.timer_start_time.elapsed().as_secs();
        if seconds_elapsed < timer_state.match_length {
            timer_state.match_length - seconds_elapsed
        } else {
            0
        }
    } else {
        timer_state.match_length
    }
}

async fn run_websocket(mut socket: WebSocket, receiver: mpsc::Receiver<TimerState>) {
    let mut timer_state = TimerState::default();
    let mut interval = time::interval(time::Duration::from_millis(100));
    loop {
        let seconds_left = get_seconds_remaining(&timer_state);
        let json_str = Json::from(json!({"time_remaining": seconds_left})).to_string();
        let msg = Message::Text(Utf8Bytes::from(json_str));
        if socket.send(msg).await.is_err() {
            break;
        }
        if let Ok(channel_val) = receiver.try_recv() {
            timer_state = channel_val;
        }
        interval.tick().await;
    }
}

impl TimerHandler {
    async fn handle_get(
        Extension(handler): Extension<SharedTimerHandler>,
    ) -> Result<impl IntoResponse, AppError> {
        let handler = handler.lock()?;
        let seconds_left = handler.get_seconds_remaining();
        Ok((
            StatusCode::OK,
            Json::from(json!({"time_remaining": seconds_left})),
        ))
    }

    async fn handle_start_timer(
        Extension(handler): Extension<SharedTimerHandler>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut handler = handler.lock()?;
        if handler.timer_state.timer_running {
            return Err((
                StatusCode::INTERNAL_SERVER_ERROR,
                "Timer is already running",
            )
                .into());
        }
        handler.start_timer();
        Ok(StatusCode::OK)
    }

    async fn handle_stop_timer(
        Extension(handler): Extension<SharedTimerHandler>,
    ) -> Result<impl IntoResponse, AppError> {
        let mut handler = handler.lock()?;
        handler.stop_timer();
        Ok(StatusCode::OK)
    }

    async fn handle_websocket_upgrade(
        Extension(handler): Extension<SharedTimerHandler>,
        ws: WebSocketUpgrade,
    ) -> Result<impl IntoResponse, AppError> {
        let (sender, receiver) = mpsc::channel();
        let mut handler = handler.lock()?;
        let _ = sender.send(handler.timer_state.clone());
        handler.senders.push(sender);
        Ok(ws.on_upgrade(|socket| run_websocket(socket, receiver)))
    }

    fn send_timer_state(&mut self) {
        let mut closed_channel_indices = vec![];
        for (i, sender) in self.senders.iter().enumerate() {
            if sender.send(self.timer_state.clone()).is_err() {
                closed_channel_indices.push(i);
            }
        }
        for i in closed_channel_indices {
            self.senders.remove(i);
        }
    }

    fn start_timer(&mut self) {
        self.timer_state.timer_running = true;
        self.timer_state.timer_start_time = Instant::now();
        self.send_timer_state();
    }

    fn stop_timer(&mut self) {
        self.timer_state.timer_running = false;
        self.send_timer_state();
    }

    fn get_seconds_remaining(&self) -> u64 {
        get_seconds_remaining(&self.timer_state)
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
