use super::Handler;
use crate::app_state::SharedAppState;
use axum::{
    Router,
    extract::State,
    http::{
        StatusCode,
        header::HeaderMap,
    },
    response::IntoResponse,
    routing::get,
};

pub struct ControlQueryHandler;

impl Handler for ControlQueryHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new().route("/controlQuery", get(Self::handle_get))
    }
}

impl ControlQueryHandler {
    async fn handle_get(
        State(app_state): State<SharedAppState>,
        headers: HeaderMap,
    ) -> impl IntoResponse {
      if !headers.contains_key("query") {
        return (StatusCode::BAD_REQUEST, String::from("Missing required header: query"))
      }
      let query = &headers["query"];
      let app_state = app_state.lock().unwrap();
      match query.to_str().unwrap() {
        "hasTeams" => (StatusCode::OK, if !app_state.teams.is_empty() { String::from("true") } else { String::from("false") }),
        "hasGame" => (StatusCode::OK, if app_state.game_description.is_some() { String::from("true") } else { String::from("false") }),
        "version" => (StatusCode::OK, String::from("0000.0-0000000")),
        _ => (StatusCode::BAD_REQUEST, String::from("Unrecognized query"))
      }
    }
}
