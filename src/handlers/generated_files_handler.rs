use super::Handler;
use crate::app_state::SharedAppState;
use crate::templates;
use axum::{
    Router,
    extract::State,
    http::{StatusCode, header},
    response::IntoResponse,
    routing::get,
};

pub struct GeneratedFilesHandler;

impl Handler for GeneratedFilesHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new()
          .route("/Scorecard.html", get(Self::get_scorecard_html))
          .route("/ScoreCalculator.js", get(Self::get_score_calculator_js))
    }
}

impl GeneratedFilesHandler {
  async fn get_scorecard_html(State(app_state): State<SharedAppState>) -> impl IntoResponse {
    let my_app_state = app_state.lock().unwrap();
    let game_description = &my_app_state.game_description;
    if let Some(game_description) = game_description {
      let rendered = templates::render_scorecard(game_description);
      match rendered {
        Ok(html) => (StatusCode::OK, [(header::CONTENT_TYPE, "text/html")], html),
        Err(_) => (StatusCode::INTERNAL_SERVER_ERROR, [(header::CONTENT_TYPE, "text/plain")], "Error rendering scorecard".to_string())
      }
    } else {
      match std::fs::read_to_string(my_app_state.resources_path.join("static_files").join("Scorecard.html")) {
        Ok(html) => return (StatusCode::OK, [(header::CONTENT_TYPE, "text/html")], html),
        Err(_) => (StatusCode::NOT_FOUND, [(header::CONTENT_TYPE, "text/plain")], "Failed to load defaul Scorecard file".to_string())
      }
    }
  }

  async fn get_score_calculator_js(State(app_state): State<SharedAppState>) -> impl IntoResponse {
    let my_app_state = app_state.lock().unwrap();
    let game_description = &my_app_state.game_description;
    if let Some(game_description) = game_description {
      let rendered = templates::render_score_calculator(game_description);
      match rendered {
        Ok(js) => (StatusCode::OK, [(header::CONTENT_TYPE, "application/javascript")], js),
        Err(e) => {
          println!("Error rendering score calculator: {:?}", e);
          (StatusCode::INTERNAL_SERVER_ERROR, [(header::CONTENT_TYPE, "text/plain")], "Error rendering scorecard".to_string())
        }
      }
    } else {
      (StatusCode::NOT_FOUND, [(header::CONTENT_TYPE, "text/plain")], "No game selected".to_string())
    }
  }
}
