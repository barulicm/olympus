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
