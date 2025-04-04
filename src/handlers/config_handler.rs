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

#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use axum::body::Body;
    use axum::http::{HeaderValue, Request, StatusCode};
    use http_body_util::BodyExt;
    use tower::ServiceExt;

    fn initialize_configs(app_state: SharedAppState) {
        let config = &mut app_state.lock().unwrap().config;
        config.competition_name = String::from("Test Competition");
        config.show_timer = true;
        config.rows_on_display = 5;
        config.display_seconds_per_page = 10;
        config.display_state = DisplayState::ShowScores;
    }

    async fn get_config_oneshot(app: Router, config_name: &str) -> String {
        let req = Request::builder()
            .method("GET")
            .uri("/config")
            .header("name", config_name)
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        String::from_utf8(body.to_vec()).unwrap()
    }

    async fn set_config_oneshot(app: Router, config_name: &str, value: HeaderValue) {
        let req = Request::builder()
            .method("PUT")
            .uri("/config")
            .header("name", config_name)
            .header("value", value)
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
    }

    #[tokio::test]
    async fn get_competition_name() {
        let app_state = create_new_shared_state();
        initialize_configs(app_state.clone());
        let app = ConfigHandler::register_routes().with_state(app_state);
        let value = get_config_oneshot(app, "competition_name").await;
        assert_eq!(value, "Test Competition");
    }

    #[tokio::test]
    async fn get_show_timer() {
        let app_state = create_new_shared_state();
        initialize_configs(app_state.clone());
        let app = ConfigHandler::register_routes().with_state(app_state);
        let value = get_config_oneshot(app, "show_timer").await;
        assert_eq!(value, "true");
    }

    #[tokio::test]
    async fn get_rows_on_display() {
        let app_state = create_new_shared_state();
        initialize_configs(app_state.clone());
        let app = ConfigHandler::register_routes().with_state(app_state);
        let value = get_config_oneshot(app, "rows_on_display").await;
        assert_eq!(value, "5");
    }

    #[tokio::test]
    async fn get_display_seconds_per_page() {
        let app_state = create_new_shared_state();
        initialize_configs(app_state.clone());
        let app = ConfigHandler::register_routes().with_state(app_state);
        let value = get_config_oneshot(app, "display_seconds_per_page").await;
        assert_eq!(value, "10");
    }

    #[tokio::test]
    async fn get_display_state() {
        let app_state = create_new_shared_state();
        initialize_configs(app_state.clone());
        let app = ConfigHandler::register_routes().with_state(app_state);
        let value = get_config_oneshot(app, "display_state").await;
        assert_eq!(value, "ShowScores");
    }

    #[tokio::test]
    async fn get_nonexistent() {
        let app_state = create_new_shared_state();
        initialize_configs(app_state.clone());
        let app = ConfigHandler::register_routes().with_state(app_state);
        let req = Request::builder()
            .method("GET")
            .uri("/config")
            .header("name", "nonexistent")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::BAD_REQUEST);
    }

    #[tokio::test]
    async fn set_competition_name() {
        let app_state = create_new_shared_state();
        let app = ConfigHandler::register_routes().with_state(app_state.clone());
        set_config_oneshot(
            app,
            "competition_name",
            HeaderValue::from_str("New Competition Name").unwrap(),
        )
        .await;
        assert_eq!(
            app_state.lock().unwrap().config.competition_name,
            "New Competition Name"
        );
    }

    #[tokio::test]
    async fn set_show_timer() {
        let app_state = create_new_shared_state();
        let app = ConfigHandler::register_routes().with_state(app_state.clone());
        set_config_oneshot(app, "show_timer", HeaderValue::from_str("true").unwrap()).await;
        assert!(app_state.lock().unwrap().config.show_timer);
    }

    #[tokio::test]
    async fn set_rows_on_display() {
        let app_state = create_new_shared_state();
        let app = ConfigHandler::register_routes().with_state(app_state.clone());
        set_config_oneshot(
            app,
            "rows_on_display",
            HeaderValue::from_str("100").unwrap(),
        )
        .await;
        assert_eq!(app_state.lock().unwrap().config.rows_on_display, 100);
    }

    #[tokio::test]
    async fn set_display_seconds_per_page() {
        let app_state = create_new_shared_state();
        let app = ConfigHandler::register_routes().with_state(app_state.clone());
        set_config_oneshot(
            app,
            "display_seconds_per_page",
            HeaderValue::from_str("1").unwrap(),
        )
        .await;
        assert_eq!(app_state.lock().unwrap().config.display_seconds_per_page, 1);
    }

    #[tokio::test]
    async fn set_display_state() {
        let app_state = create_new_shared_state();
        let app = ConfigHandler::register_routes().with_state(app_state.clone());
        set_config_oneshot(
            app,
            "display_state",
            HeaderValue::from_str("FllLogo").unwrap(),
        )
        .await;
        assert_eq!(
            app_state.lock().unwrap().config.display_state,
            DisplayState::FllLogo
        );
    }

    #[tokio::test]
    async fn set_nonexistent() {
        let app_state = create_new_shared_state();
        initialize_configs(app_state.clone());
        let app = ConfigHandler::register_routes().with_state(app_state);
        let req = Request::builder()
            .method("PUT")
            .uri("/config")
            .header("name", "nonexistent")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::BAD_REQUEST);
    }
}
