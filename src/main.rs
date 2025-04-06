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
        .map(std::path::PathBuf::from)
        .unwrap_or(resource_management::get_resource_directory_path());
    println!("Using resources path: {}", resources_path.display());
    resource_management::initialize_resources_directory(&resources_path, args.init_resources)
        .expect("Failed to initialize resources directory.");

    let app_state = app_state::create_new_shared_state();
    app_state
        .lock()
        .expect("Failed to lock app state to set resources path.")
        .resources_path = resources_path.clone();

    let panic_handler_app_state = app_state.clone();
    std::panic::set_hook(Box::new(move |panic_info| {
        handle_panic(panic_info, &panic_handler_app_state);
    }));

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

fn handle_panic(panic_info: &std::panic::PanicHookInfo, app_state: &SharedAppState) {
    eprintln!("A panic occurred: {}", panic_info);
    let app_state = app_state.lock();
    if let Ok(app_state) = app_state {
        let app_state_string = serde_json::to_string_pretty(&*app_state)
            .unwrap_or(String::from("Failed to render app state."));
        let file_name = format!(
            "session-backup-{}.json",
            chrono::Local::now().format("%Y-%m-%d-%H-%M-%S")
        );
        println!("Saving session backup to {}", file_name);
        let res = std::fs::write(file_name, app_state_string);
        if let Err(e) = res {
            eprintln!("Feiled to write session backup. Error: {}", e);
        }
    } else {
        eprintln!("Failed to lock app state during panic handling.");
    }
    std::process::exit(1);
}
