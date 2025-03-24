use super::Handler;
use crate::app_state::{SharedAppState, DisplayState};
use axum::{
    Router,
    extract::State,
    http::{StatusCode, header::HeaderMap},
    response::IntoResponse,
    routing::{get, put},
};

pub struct ConfigHandler;

impl Handler for ConfigHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new()
            .route("/config", get(Self::handle_get))
            .route("/config", put(Self::handle_put))
    }
}

impl ConfigHandler {
    async fn handle_get(
        headers: HeaderMap,
        State(app_state): State<SharedAppState>,
    ) -> impl IntoResponse {
        if !headers.contains_key("name") {
            return (StatusCode::BAD_REQUEST, "Missing name header".to_string());
        }
        let name = headers.get("name").unwrap().to_str().unwrap();
        let config = &app_state.lock().unwrap().config;
        let value = match name {
            "competition_name" => Some(config.competition_name.clone()),
            "show_timer" => Some(config.show_timer.to_string()),
            "rows_on_display" => Some(config.rows_on_display.to_string()),
            "display_seconds_per_page" => Some(config.display_seconds_per_page.to_string()),
            "display_state" => Some(config.display_state.to_string()),
            _ => None,
        };
        match value {
            Some(value) => (StatusCode::OK, value),
            None => (StatusCode::BAD_REQUEST, "Config not found".to_string()),
        }
    }

    async fn handle_put(
        headers: HeaderMap,
        State(app_state): State<SharedAppState>,
    ) -> impl IntoResponse {
      if !headers.contains_key("name") {
        return (StatusCode::BAD_REQUEST, "Missing name header".to_string());
      }
      let name = headers.get("name").unwrap().to_str().unwrap();
      let config = &mut app_state.lock().unwrap().config;
      let result = match name {
          "competition_name" => {
              let value = headers.get("value").unwrap().to_str().unwrap();
              config.competition_name = value.to_string();
              Ok(())
          },
          "show_timer" => {
              let value = headers.get("value").unwrap().to_str().unwrap();
              config.show_timer = value.parse::<bool>().unwrap();
              Ok(())
          },
          "rows_on_display" => {
              let value = headers.get("value").unwrap().to_str().unwrap();
              config.rows_on_display = value.parse::<usize>().unwrap();
              Ok(())
          },
          "display_seconds_per_page" => {
              let value = headers.get("value").unwrap().to_str().unwrap();
              config.display_seconds_per_page = value.parse::<u64>().unwrap();
              Ok(())
          },
          "display_state" => {
              let value = headers.get("value").unwrap().to_str().unwrap();
              let value = match value {
                  "ShowScores" => Ok(DisplayState::ShowScores),
                  "Blackout" => Ok(DisplayState::Blackout),
                  "FllLogo" => Ok(DisplayState::FllLogo),
                  "Sponsors" => Ok(DisplayState::Sponsors),
                  _ => Err(()),
              };
              match value {
                Ok(v) => {
                  config.display_state = v;
                  Ok(())
                },
                Err(_) => Err(()),
              }
          },
          _ => Err(()),
      };
      match result {
        Ok(_) => (StatusCode::OK, "Config updated".to_string()),
        Err(_) => (StatusCode::BAD_REQUEST, "Error updating config".to_string()),
      }
    }
}
