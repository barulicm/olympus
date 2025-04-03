use super::Handler;
use crate::app_error::AppError;
use crate::app_state::{DisplayState, SharedAppState};
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
    ) -> Result<impl IntoResponse, AppError> {
        let name = headers
            .get("name")
            .ok_or((StatusCode::BAD_REQUEST, "Missing name header"))?
            .to_str()
            .map_err(|_| {
                (
                    StatusCode::BAD_REQUEST,
                    "'name' header is not an ASCII string",
                )
            })?;
        let config = &app_state.lock()?.config;
        let value = match name {
            "competition_name" => Some(config.competition_name.clone()),
            "show_timer" => Some(config.show_timer.to_string()),
            "rows_on_display" => Some(config.rows_on_display.to_string()),
            "display_seconds_per_page" => Some(config.display_seconds_per_page.to_string()),
            "display_state" => Some(config.display_state.to_string()),
            _ => None,
        };
        match value {
            Some(value) => Ok((StatusCode::OK, value)),
            None => Err((StatusCode::BAD_REQUEST, "Config not found").into()),
        }
    }

    async fn handle_put(
        headers: HeaderMap,
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let name = headers
            .get("name")
            .ok_or((StatusCode::BAD_REQUEST, "Missing name header"))?
            .to_str()
            .map_err(|_| {
                (
                    StatusCode::BAD_REQUEST,
                    "'name' header is not an ASCII string",
                )
            })?;
        let value = headers
            .get("value")
            .ok_or((StatusCode::BAD_REQUEST, "Missing value header"))?
            .to_str()
            .map_err(|_| {
                (
                    StatusCode::BAD_REQUEST,
                    "'value' header is not an ASCII string",
                )
            })?;
        let config = &mut app_state.lock()?.config;
        let result = match name {
            "competition_name" => {
                config.competition_name = value.to_string();
                Ok(())
            }
            "show_timer" => {
                config.show_timer = value
                    .parse::<bool>()
                    .map_err(|_| (StatusCode::BAD_REQUEST, "Could not parse bool from value"))?;
                Ok(())
            }
            "rows_on_display" => {
                config.rows_on_display = value
                    .parse::<usize>()
                    .map_err(|_| (StatusCode::BAD_REQUEST, "Could not parse usize from value"))?;
                Ok(())
            }
            "display_seconds_per_page" => {
                config.display_seconds_per_page = value
                    .parse::<u64>()
                    .map_err(|_| (StatusCode::BAD_REQUEST, "Could not parse u64 from value"))?;
                Ok(())
            }
            "display_state" => {
                let value = match value {
                    "ShowScores" => Ok(DisplayState::ShowScores),
                    "Blackout" => Ok(DisplayState::Blackout),
                    "FllLogo" => Ok(DisplayState::FllLogo),
                    "Sponsors" => Ok(DisplayState::Sponsors),
                    _ => Err((
                        StatusCode::BAD_REQUEST,
                        format!("Unrecognized display state: {}", value),
                    )),
                };
                match value {
                    Ok(v) => {
                        config.display_state = v;
                        Ok(())
                    }
                    Err(e) => Err(e),
                }
            }
            _ => Err((
                StatusCode::BAD_REQUEST,
                format!("Unrecognized config name: {}", name),
            )),
        };
        match result {
            Ok(_) => Ok((StatusCode::OK, String::from("Config updated"))),
            Err(e) => Err(e.into()),
        }
    }
}
