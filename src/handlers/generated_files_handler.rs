use super::Handler;
use crate::app_error::AppError;
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
    async fn get_scorecard_html(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let my_app_state = app_state.lock()?;
        let game_description = &my_app_state.game_description;
        if let Some(game_description) = game_description {
            let rendered = templates::render_scorecard(game_description);
            match rendered {
                Ok(html) => Ok((StatusCode::OK, [(header::CONTENT_TYPE, "text/html")], html)),
                Err(_) => Err((
                    StatusCode::INTERNAL_SERVER_ERROR,
                    "Error rendering scorecard",
                )
                    .into()),
            }
        } else {
            match std::fs::read_to_string(
                my_app_state
                    .resources_path
                    .join("static_files")
                    .join("Scorecard.html"),
            ) {
                Ok(html) => Ok((StatusCode::OK, [(header::CONTENT_TYPE, "text/html")], html)),
                Err(_) => Err((
                    StatusCode::NOT_FOUND,
                    "Failed to load defaul Scorecard file",
                )
                    .into()),
            }
        }
    }

    async fn get_score_calculator_js(
        State(app_state): State<SharedAppState>,
    ) -> Result<impl IntoResponse, AppError> {
        let my_app_state = app_state.lock()?;
        let game_description = &my_app_state.game_description;
        if let Some(game_description) = game_description {
            let rendered = templates::render_score_calculator(game_description);
            match rendered {
                Ok(js) => Ok((
                    StatusCode::OK,
                    [(header::CONTENT_TYPE, "application/javascript")],
                    js,
                )),
                Err(_) => Err((
                    StatusCode::INTERNAL_SERVER_ERROR,
                    "Error rendering scorecard",
                )
                    .into()),
            }
        } else {
            Err((StatusCode::NOT_FOUND, "No game selected").into())
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use crate::game_description::GameDescription;
    use axum::body::Body;
    use axum::http::{Request, StatusCode, header::HeaderValue};
    use http_body_util::BodyExt;
    use tower::ServiceExt;

    #[tokio::test]
    async fn scorecard() {
        let app_state = create_new_shared_state();
        let game_desc_path = std::env::current_dir()
            .unwrap()
            .join("src/test_resources/game_configs/test_game.json");
        app_state.lock().unwrap().game_description =
            Some(GameDescription::from_path(&game_desc_path).unwrap());
        let app = GeneratedFilesHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/Scorecard.html")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        assert_eq!(
            response.headers().get(header::CONTENT_TYPE),
            Some(HeaderValue::from_static("text/html")).as_ref()
        );
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let expected_bytes = include_bytes!("../../src/test_resources/expected_scorecard.html");
        assert_eq!(&body[..], expected_bytes);
    }

    #[tokio::test]
    async fn score_calculator() {
        let app_state = create_new_shared_state();
        let game_desc_path = std::env::current_dir()
            .unwrap()
            .join("src/test_resources/game_configs/test_game.json");
        app_state.lock().unwrap().game_description =
            Some(GameDescription::from_path(&game_desc_path).unwrap());
        let app = GeneratedFilesHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/ScoreCalculator.js")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        assert_eq!(
            response.headers().get(header::CONTENT_TYPE),
            Some(HeaderValue::from_static("application/javascript")).as_ref()
        );
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let expected_bytes =
            include_bytes!("../../src/test_resources/expected_score_calculator.js");
        assert_eq!(&body[..], expected_bytes);
    }
}
