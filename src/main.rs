mod app_error;
mod app_state;
mod arguments;
mod game_description;
mod handlers;
mod resource_management;
mod templates;
mod version;

use app_state::SharedAppState;
use axum::Router;
use clap::Parser;
use std::time::Duration;
use tokio::signal;
use tower_http::{services::ServeDir, timeout::TimeoutLayer};

#[tokio::main]
async fn main() {
    println!("Olympus {}", version::get_version());

    let args = arguments::Arguments::parse();

    let resources_path = args
        .resources_dir
        .map(|s| std::path::PathBuf::from(s))
        .unwrap_or(resource_management::get_resource_directory_path());
    println!("Using resources path: {}", resources_path.display());
    resource_management::initialize_resources_directory(&resources_path, args.init_resources)
        .expect("Failed to initialize resources directory.");

    let app_state = app_state::create_new_shared_state();
    app_state
        .lock()
        .expect("Failed to lock app state to set resources path.")
        .resources_path = resources_path.clone();

    let router = Router::<SharedAppState>::new()
        .merge(handlers::register_all_handlers())
        .with_state(app_state)
        .fallback_service(ServeDir::new(resources_path.join("static_files")))
        .layer((TimeoutLayer::new(Duration::from_secs(10)),));

    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000")
        .await
        .expect("Failed to bind TCP listener.");

    axum::serve(listener, router)
        .with_graceful_shutdown(shutdown_signal())
        .await
        .expect("Failed to start the server.");
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
