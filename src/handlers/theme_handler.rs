use std::path::PathBuf;

use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use axum::{
    Router,
    extract::State,
    http::StatusCode,
    response::IntoResponse,
    routing::{get, put},
};
use serde::Deserialize;
use serde_json::{Value, json};

const THEME_DIR_NAME : &str = "themes";
const METADATA_FILENAME : &str = "theme.json";

#[derive(Clone)]
pub struct ThemeHandler;

#[derive(Deserialize)]
struct ThemeMetadata {
    display_name: String
}

impl ThemeMetadata {
    fn from_path(path: &std::path::PathBuf) -> std::io::Result<Self> {
        let file_contents = std::fs::read_to_string(path)?;
        let json_value = serde_json::from_str(&file_contents.as_str())?;
        Ok(json_value)
    }
}

impl Handler for ThemeHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new()
            .route("/theme/available_themes", get(Self::get_available_themes))
            .route("/theme/meta", get(Self::get_theme_metadata))
            .route("/theme/style.css", get(Self::get_style))
            .route("/theme/choose", put(Self::set_theme))
    }
}

impl ThemeHandler {
    fn is_valid_theme_path(path: &PathBuf) -> bool {
        path.is_dir() && path.join(METADATA_FILENAME).exists()
    }

    async fn get_available_themes(State(app_state): State<SharedAppState>) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let themes_dir = app_state.resources_path.join(THEME_DIR_NAME);
        let available_games = std::fs::read_dir(themes_dir)?
            .filter(|e| {
                let path = e.as_ref().map(|v| v.path()).unwrap_or_default();
                ThemeHandler::is_valid_theme_path(&path)
            })
            .filter_map(|e| {
                if let Ok(e) = e {
                    let theme_name = e.file_name();
                    let metadata_path = e.path().join(METADATA_FILENAME);
                    let metadata = ThemeMetadata::from_path(&metadata_path);
                    if let Ok(metadata) = metadata {
                        Some(json!({
                            "name": theme_name,
                            "display_name": metadata.display_name,
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

    async fn get_theme_metadata(State(app_state): State<SharedAppState>) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let theme_dir = app_state.resources_path.join(THEME_DIR_NAME).join(&app_state.display_theme);
        let metadata_path = theme_dir.join(METADATA_FILENAME);
        let metadata = ThemeMetadata::from_path(&metadata_path)?;
        Ok((StatusCode::OK, json!({
            "name": app_state.display_theme,
            "display_name": metadata.display_name
        }).to_string()))
    }

    async fn get_style(State(app_state): State<SharedAppState>) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let theme_dir = app_state.resources_path.join(THEME_DIR_NAME).join(&app_state.display_theme);
        let file_contents = std::fs::read_to_string(theme_dir.join("style.css"))?;
        Ok((StatusCode::OK, file_contents))
    }

    async fn set_theme(State(app_state): State<SharedAppState>, body: String) -> Result<impl IntoResponse, AppError> {
        let mut app_state = app_state.lock()?;
        let themes_dir = app_state.resources_path.join(THEME_DIR_NAME);
        for entry in (std::fs::read_dir(themes_dir)?).flatten() {
            let path = entry.path();
            if !ThemeHandler::is_valid_theme_path(&path) {
                continue;
            }
            if let Some(theme_name) = entry.file_name().to_str() {
                if theme_name == body {
                app_state.display_theme = body;
                return Ok((StatusCode::OK, ""));
                }
            }
        }
        Ok((StatusCode::BAD_REQUEST, "No theme with requested name."))
    }
}

// TODO: add tests
