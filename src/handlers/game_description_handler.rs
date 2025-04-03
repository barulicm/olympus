use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use crate::game_description::GameDescription;
use axum::{
    Router,
    extract::State,
    http::{StatusCode, header},
    response::IntoResponse,
    routing::{get, put},
};
use serde_json::{Value, json};

#[derive(Clone)]
pub struct GameDescriptionHandler;

impl Handler for GameDescriptionHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new()
            .route("/game/available_games", get(Self::get_available_games))
            .route("/game/meta", get(Self::get_game_metadata))
            .route("/game/logo", get(Self::get_game_logo))
            .route("/game/choose", put(Self::set_game_selection))
    }
}

impl GameDescriptionHandler {
    async fn get_available_games(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let game_configs_dir = app_state.resources_path.join("game_configs");
        let available_games = std::fs::read_dir(game_configs_dir)?
            .filter(|e| {
                let path = e.as_ref().map(|v| v.path()).unwrap_or_default();
                path.is_file() && path.extension().unwrap_or_default() == "json"
            })
            .filter_map(|e| {
                if let Ok(e) = e {
                    let description = GameDescription::from_path(&e.path());
                    if let Ok(description) = description {
                        Some(json!({
                            "name": description.name,
                            "description": description.description,
                        }))
                    } else {
                        None
                    }
                } else {
                    None
                }
            })
            .collect::<Vec<Value>>();
        Ok((StatusCode::OK, serde_json::to_string(&available_games)?))
    }

    async fn get_game_metadata(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let game_description = &app_state.game_description;
        if let Some(game_description) = game_description {
            Ok((
                StatusCode::OK,
                json!({"name": game_description.name, "description": game_description.description})
                    .to_string(),
            ))
        } else {
            Err(AppError::new(
                StatusCode::NO_CONTENT,
                String::from("No game selected"),
            ))
        }
    }

    async fn get_game_logo(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let game_description = &app_state.game_description;
        if let Some(game_description) = game_description {
            let logo_path = app_state
                .resources_path
                .join("game_configs")
                .join(&game_description.logo);
            match std::fs::read(logo_path) {
                Ok(logo) => {
                    let mime_type =
                        mime_guess::from_path(&game_description.logo).first_or_octet_stream();
                    Ok((
                        StatusCode::OK,
                        [(header::CONTENT_TYPE, mime_type.to_string())],
                        logo,
                    ))
                }
                Err(_) => Err((StatusCode::INTERNAL_SERVER_ERROR, "Failed to load logo").into()),
            }
        } else {
            Err((StatusCode::NOT_FOUND, "No game selected").into())
        }
    }

    async fn set_game_selection(
        State(app_state): State<SharedAppState>,
        body: String,
    ) -> Result<impl IntoResponse, AppError> {
        let mut app_state = app_state.lock()?;
        let game_configs_dir = app_state.resources_path.join("game_configs");
        for entry in (std::fs::read_dir(game_configs_dir)?).flatten() {
            let path = entry.path();
            if !(path.is_file() && path.extension().unwrap_or_default() == "json") {
                continue;
            }
            let game_description = GameDescription::from_path(&path);
            if let Ok(game_description) = game_description {
                if game_description.name == body {
                    app_state.game_description = Some(game_description);
                    return Ok((StatusCode::OK, ""));
                }
            }
        }
        Ok((StatusCode::BAD_REQUEST, "No game with requested name."))
    }
}
