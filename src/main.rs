mod app_state;
mod game_description;
mod handlers;
mod templates;

use app_state::SharedAppState;
use axum::Router;

#[tokio::main]
async fn main() {
    let app_state = app_state::create_new_shared_state();
    let router = Router::<SharedAppState>::new()
        .merge(handlers::register_all_handlers())
        .with_state(app_state);
    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();
    axum::serve(listener, router).await.unwrap();
}
