use super::Handler;
use crate::app_error::AppError;
use crate::app_state::SharedAppState;
use crate::version::get_version;
use axum::{
    Router,
    extract::State,
    http::{StatusCode, header::HeaderMap},
    response::IntoResponse,
    routing::get,
};

pub struct ControlQueryHandler;

impl Handler for ControlQueryHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new().route("/controlQuery", get(Self::handle_get))
    }
}

impl ControlQueryHandler {
    async fn handle_get(
        State(app_state): State<SharedAppState>,
        headers: HeaderMap,
    ) -> Result<impl IntoResponse, AppError> {
        let query = headers
            .get("query")
            .ok_or((StatusCode::BAD_REQUEST, "Missing query header"))?
            .to_str()
            .map_err(|_| {
                (
                    StatusCode::BAD_REQUEST,
                    "'query' header is not an ASCII string",
                )
            })?;
        let app_state = app_state.lock()?;
        match query {
            "hasTeams" => Ok((
                StatusCode::OK,
                if !app_state.teams.is_empty() {
                    String::from("true")
                } else {
                    String::from("false")
                },
            )),
            "hasGame" => Ok((
                StatusCode::OK,
                if app_state.game_description.is_some() {
                    String::from("true")
                } else {
                    String::from("false")
                },
            )),
            "version" => Ok((StatusCode::OK, String::from(get_version()))),
            _ => Err((StatusCode::BAD_REQUEST, "Unrecognized query").into()),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::{Team, create_new_shared_state};
    use crate::game_description::GameDescription;
    use axum::body::Body;
    use axum::http::{Request, StatusCode};
    use axum::routing::RouterIntoService;
    use http_body_util::BodyExt;
    use tower::{Service, ServiceExt};

    async fn get_control_query(app: &mut RouterIntoService<Body>, query: &str) -> String {
        let req = Request::builder()
            .method("GET")
            .uri("/controlQuery")
            .header("query", query)
            .body(Body::empty())
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        String::from_utf8(body.to_vec()).unwrap()
    }

    #[tokio::test]
    async fn has_teams() {
        let app_state = create_new_shared_state();
        let mut app = ControlQueryHandler::register_routes()
            .with_state(app_state.clone())
            .into_service();

        let value = get_control_query(&mut app, "hasTeams").await;
        assert_eq!(value, "false", "Initial state should have no teams");

        app_state
            .lock()
            .unwrap()
            .teams
            .push(Team::new(String::from("1234"), String::from("Test Team")));

        let value = get_control_query(&mut app, "hasTeams").await;
        assert_eq!(
            value, "true",
            "Query should return 'true' after adding a team"
        );
    }

    #[tokio::test]
    async fn has_game() {
        let app_state = create_new_shared_state();
        let mut app = ControlQueryHandler::register_routes()
            .with_state(app_state.clone())
            .into_service();

        let value = get_control_query(&mut app, "hasGame").await;
        assert_eq!(value, "false", "Initial state should have no game");

        app_state.lock().unwrap().game_description = Some(GameDescription {
            name: String::from("Test Game"),
            description: String::from("This is a test game"),
            logo: String::new(),
            missions: vec![],
        });

        let value = get_control_query(&mut app, "hasGame").await;
        assert_eq!(
            value, "true",
            "Query should return 'true' after setting the game"
        );
    }

    #[tokio::test]
    async fn version() {
        let app_state = create_new_shared_state();
        let mut app = ControlQueryHandler::register_routes()
            .with_state(app_state)
            .into_service();
        let value = get_control_query(&mut app, "version").await;
        assert!(
            !value.is_empty(),
            "Version query should return something, the specific value will change."
        );
        // TODO(barulicm): could validate version format with regex?
    }
}
