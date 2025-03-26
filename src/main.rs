mod app_state;
mod game_description;
mod handlers;
mod templates;

use app_state::SharedAppState;
use axum::Router;
use std::time::Duration;
use tokio::signal;
use tower_http::{services::ServeDir, timeout::TimeoutLayer};

#[tokio::main]
async fn main() {
    let resources_path = std::env::current_exe()
        .unwrap()
        .parent()
        .unwrap()
        .join("resources");
    let app_state = app_state::create_new_shared_state();
    app_state.lock().unwrap().resources_path = resources_path.clone();
    
    let router = Router::<SharedAppState>::new()
        .merge(handlers::register_all_handlers())
        .with_state(app_state)
        .fallback_service(ServeDir::new(resources_path.join("static_files")))
        .layer((TimeoutLayer::new(Duration::from_secs(10)),));
    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();
    axum::serve(listener, router)
        .with_graceful_shutdown(shutdown_signal())
        .await
        .unwrap();
    // TODO backup app state on uncaught exceptions & graceful shutdown
}

async fn shutdown_signal() {
    let ctrl_c = async {
        signal::ctrl_c()
            .await
            .expect("failed to install Ctrl+C handler");
    };

    #[cfg(unix)]
    let terminate = async {
        signal::unix::signal(signal::unix::SignalKind::terminate())
            .expect("failed to install signal handler")
            .recv()
            .await;
    };

    #[cfg(not(unix))]
    let terminate = std::future::pending::<()>();

    tokio::select! {
        _ = ctrl_c => {},
        _ = terminate => {},
    }
}
