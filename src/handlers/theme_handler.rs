use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use axum::{
    Router,
    extract::State,
    http::{StatusCode, header},
    response::IntoResponse,
    routing::{get, put},
};
use serde::Deserialize;
use serde_json::{Value, json};
use std::path::Path;

const THEME_DIR_NAME: &str = "themes";
const METADATA_FILENAME: &str = "theme.json";

#[derive(Clone)]
pub struct ThemeHandler;

#[derive(Deserialize)]
struct ThemeMetadata {
    display_name: String,
}

impl ThemeMetadata {
    fn from_path(path: &std::path::PathBuf) -> std::io::Result<Self> {
        let file_contents = std::fs::read_to_string(path)?;
        let json_value = serde_json::from_str(file_contents.as_str())?;
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
    fn is_valid_theme_path(path: &Path) -> bool {
        path.is_dir() && path.join(METADATA_FILENAME).exists()
    }

    async fn get_available_themes(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let themes_dir = app_state.resources_path.join(THEME_DIR_NAME);
        let available_games = std::fs::read_dir(themes_dir)?
            .filter(|e| {
                let path = e.as_ref().map(|v| v.path()).unwrap_or_default();
                ThemeHandler::is_valid_theme_path(&path)
            })
            .filter_map(|e| {
                if let Ok(e) = e {
                    if let Ok(theme_name) = e.file_name().into_string() {
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
                } else {
                    None
                }
            })
            .collect::<Vec<Value>>();
        Ok((StatusCode::OK, serde_json::to_string(&available_games)?))
    }

    async fn get_theme_metadata(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let theme_dir = app_state
            .resources_path
            .join(THEME_DIR_NAME)
            .join(&app_state.display_theme);
        let metadata_path = theme_dir.join(METADATA_FILENAME);
        let metadata = ThemeMetadata::from_path(&metadata_path)?;
        Ok((
            StatusCode::OK,
            json!({
                "name": app_state.display_theme,
                "display_name": metadata.display_name
            })
            .to_string(),
        ))
    }

    async fn get_style(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        let theme_dir = app_state
            .resources_path
            .join(THEME_DIR_NAME)
            .join(&app_state.display_theme);
        let file_contents = std::fs::read_to_string(theme_dir.join("style.css"))?;
        Ok((
            StatusCode::OK,
            [(header::CONTENT_TYPE, "text/css")],
            file_contents,
        ))
    }

    async fn set_theme(
        State(app_state): State<SharedAppState>,
        body: String,
    ) -> Result<impl IntoResponse, AppError> {
        let mut app_state = app_state.lock()?;
        let themes_dir = app_state.resources_path.join(THEME_DIR_NAME);
        for entry in (std::fs::read_dir(themes_dir)?).flatten() {
            let path = entry.path();
            if !ThemeHandler::is_valid_theme_path(&path) {
                continue;
            }
            if let Some(theme_name) = entry.file_name().to_str()
                && theme_name == body
            {
                app_state.display_theme = body;
                return Ok((StatusCode::OK, ""));
            }
        }
        Ok((StatusCode::BAD_REQUEST, "No theme with requested name."))
    }
}

// TODO: add tests
#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use axum::body::Body;
    use axum::http::{Request, StatusCode, header};
    use http_body_util::BodyExt;
    use serde_json::{Value, json};
    use tower::ServiceExt;

    #[tokio::test]
    async fn available_themes() {
        let app_state = create_new_shared_state();
        app_state.lock().unwrap().resources_path = std::env::current_dir()
            .unwrap()
            .join("src")
            .join("test_resources");
        let app = ThemeHandler::register_routes().with_state(app_state);
        let req = Request::builder()
            .method("GET")
            .uri("/theme/available_themes")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(
            body,
            json!([
                {
                    "name": "test_theme",
                    "display_name": "Test Theme"
                }
            ])
        );
    }

    #[tokio::test]
    async fn metadata() {
        let app_state = create_new_shared_state();
        let resource_path = std::env::current_dir()
            .unwrap()
            .join("src")
            .join("test_resources");
        app_state.lock().unwrap().resources_path = resource_path.clone();
        app_state.lock().unwrap().display_theme = "test_theme".to_string();
        let app = ThemeHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/theme/meta")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(
            body,
            json!(
                {
                    "name": "test_theme",
                    "display_name": "Test Theme"
                }
            )
        );
    }

    #[tokio::test]
    async fn style() {
        let app_state = create_new_shared_state();
        let resource_path = std::env::current_dir()
            .unwrap()
            .join("src")
            .join("test_resources");
        app_state.lock().unwrap().resources_path = resource_path.clone();
        app_state.lock().unwrap().display_theme = "test_theme".to_string();
        let app = ThemeHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/theme/style.css")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        assert_eq!(
            response
                .headers()
                .get(header::CONTENT_TYPE)
                .unwrap()
                .to_str()
                .unwrap(),
            "text/css"
        );
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let expected_bytes = include_bytes!("../../src/test_resources/themes/test_theme/style.css");
        assert_eq!(&body[..], expected_bytes);
    }

    #[tokio::test]
    async fn choose() {
        let app_state = create_new_shared_state();
        let resource_path = std::env::current_dir()
            .unwrap()
            .join("src")
            .join("test_resources");
        app_state.lock().unwrap().resources_path = resource_path.clone();
        let app = ThemeHandler::register_routes().with_state(app_state.clone());

        let req = Request::builder()
            .method("PUT")
            .uri("/theme/choose")
            .body(Body::from("test_theme"))
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        assert_eq!(app_state.lock().unwrap().display_theme, "test_theme");
    }
}
