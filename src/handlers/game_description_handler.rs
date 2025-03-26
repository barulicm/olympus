use super::Handler;
use crate::app_state::SharedAppState;
use crate::game_description::GameDescription;
use axum::{
    Router,
    extract::State,
    http::{StatusCode, header},
    response::{IntoResponse, Response},
    routing::{get, put},
};
use mime_guess;
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
    async fn get_available_games(State(app_state): State<SharedAppState>) -> impl IntoResponse {
        let app_state = app_state.lock().unwrap();
        let game_configs_dir = app_state.resources_path.join("game_configs");
        let available_games = std::fs::read_dir(game_configs_dir)
            .unwrap()
            .filter(|e| {
                let path = e.as_ref().unwrap().path();
                path.is_file() && path.extension().unwrap() == "json"
            })
            .map(|e| {
                let description = GameDescription::from_path(&e.unwrap().path()).unwrap();
                json!({
                    "name": description.name,
                    "description": description.description,
                })
            })
            .collect::<Vec<Value>>();
        (
            StatusCode::OK,
            serde_json::to_string(&available_games).unwrap(),
        )
    }

    async fn get_game_metadata(State(app_state): State<SharedAppState>) -> impl IntoResponse {
        let app_state = app_state.lock().unwrap();
        let game_description = &app_state.game_description;
        if let Some(game_description) = game_description {
            (
                StatusCode::OK,
                json!({"name": game_description.name, "description": game_description.description})
                    .to_string(),
            )
        } else {
            (StatusCode::NO_CONTENT, "No game selected".to_string())
        }
    }

    async fn get_game_logo(State(app_state): State<SharedAppState>) -> Response {
        let app_state = app_state.lock().unwrap();
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
                    (
                        StatusCode::OK,
                        [(header::CONTENT_TYPE, mime_type.to_string())],
                        logo,
                    )
                        .into_response()
                }
                Err(_) => {
                    (StatusCode::INTERNAL_SERVER_ERROR, "Failed to load logo").into_response()
                }
            }
        } else {
            (StatusCode::NOT_FOUND, "No game selected".to_string()).into_response()
        }
    }

    async fn set_game_selection(
        State(app_state): State<SharedAppState>,
        body: String,
    ) -> impl IntoResponse {
        let mut app_state = app_state.lock().unwrap();
        let game_configs_dir = app_state.resources_path.join("game_configs");
        for entry in std::fs::read_dir(game_configs_dir).unwrap() {
            let path = entry.unwrap().path();
            if !(path.is_file() && path.extension().unwrap() == "json") {
                continue;
            }
            let game_description = GameDescription::from_path(&path).unwrap();
            if game_description.name == body {
                app_state.game_description = Some(game_description);
                return (StatusCode::OK, String::new());
            }
        }
        (
            StatusCode::BAD_REQUEST,
            String::from("No game with requested name."),
        )
    }
}
