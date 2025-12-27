use super::Handler;
use crate::app_error::AppError;
use crate::app_state::{SharedAppState, Team};
use axum::{
    Router,
    extract::{Json, Path, State},
    http::{StatusCode, header},
    response::IntoResponse,
    routing::{get, put},
};
use serde_json::Value;

pub struct TeamHandler;

impl Handler for TeamHandler {
    fn register_routes() -> Router<SharedAppState> {
        Router::new()
            .route("/team/add", put(Self::handle_add_team))
            .route("/team/remove", put(Self::handle_remove_team))
            .route("/team/edit", put(Self::handle_edit_team))
            .route("/team/{team_number}", get(Self::handle_get))
    }
}

impl TeamHandler {
    async fn handle_get(
        State(app_state): State<SharedAppState>,
        Path(team_number): Path<String>,
    ) -> Result<impl IntoResponse, AppError> {
        let app_state = app_state.lock()?;
        if team_number == "all" {
            let response_body = serde_json::to_string(&app_state.teams).map_err(|e| {
                AppError::new(
                    StatusCode::INTERNAL_SERVER_ERROR,
                    format!("Failed to serialize teams: {}", e),
                )
            })?;
            Ok((
                StatusCode::OK,
                [(header::CONTENT_TYPE, "application/json")],
                response_body,
            ))
        } else {
            let team = app_state
                .teams
                .iter()
                .find(|t| t.number == team_number)
                .ok_or(AppError::new(
                    StatusCode::BAD_REQUEST,
                    format!("No team with number '{}'", team_number),
                ))?;
            let response_body = serde_json::to_string(&team).map_err(|e| {
                AppError::new(
                    StatusCode::INTERNAL_SERVER_ERROR,
                    format!("Failed to serialize team: {}", e),
                )
            })?;
            Ok((
                StatusCode::OK,
                [(header::CONTENT_TYPE, "application/json")],
                response_body,
            ))
        }
    }

    async fn handle_add_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let team_name = body
            .get("name")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid team name."),
            ))?
            .to_string();
        let team_number = body
            .get("number")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid team number."),
            ))?
            .to_string();
        let new_team = Team::new(team_number, team_name);
        if new_team.number.is_empty() {
            return Err((
                StatusCode::BAD_REQUEST,
                String::from("Team number must not be empty"),
            )
                .into());
        }
        let mut app_state = app_state.lock()?;
        let old_team = app_state.teams.iter().find(|t| t.number == new_team.number);
        if old_team.is_some() {
            return Err((
                StatusCode::BAD_REQUEST,
                "Another team is already using the new team number.",
            )
                .into());
        }
        app_state.teams.push(new_team);
        Ok(StatusCode::OK)
    }

    async fn handle_remove_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let team_number = body
            .get("number")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid team number."),
            ))?;
        let mut app_state = app_state.lock()?;
        let found_team_position = app_state
            .teams
            .iter()
            .position(|t| t.number == team_number)
            .ok_or(AppError::new(
                StatusCode::NOT_FOUND,
                format!("No team with number '{}'", team_number),
            ))?;
        app_state.teams.remove(found_team_position);
        Ok((StatusCode::OK, "Remove team successful."))
    }

    async fn handle_edit_team(
        State(app_state): State<SharedAppState>,
        Json(body): Json<Value>,
    ) -> Result<impl IntoResponse, AppError> {
        let old_team_number =
            body.get("oldTeamNumber")
                .and_then(|v| v.as_str())
                .ok_or(AppError::new(
                    StatusCode::BAD_REQUEST,
                    String::from("Missing or invalid old team number."),
                ))?;
        let new_team_number =
            body.get("newTeamNumber")
                .and_then(|v| v.as_str())
                .ok_or(AppError::new(
                    StatusCode::BAD_REQUEST,
                    String::from("Missing or invalid new team number."),
                ))?;
        if new_team_number.is_empty() {
            return Err((StatusCode::BAD_REQUEST, "Team number must not be empty.").into());
        }
        let mut app_state = app_state.lock()?;
        if new_team_number != old_team_number
            && app_state.teams.iter().any(|t| t.number == new_team_number)
        {
            return Err((
                StatusCode::BAD_REQUEST,
                "Another team is already using the new team number.",
            )
                .into());
        }
        let team = app_state
            .teams
            .iter_mut()
            .find(|t| t.number == old_team_number)
            .ok_or(AppError::new(
                StatusCode::NOT_FOUND,
                format!("No team with number '{}'", old_team_number),
            ))?;
        let new_team_name = body
            .get("newTeamName")
            .and_then(|v| v.as_str())
            .ok_or(AppError::new(
                StatusCode::BAD_REQUEST,
                String::from("Missing or invalid new team number."),
            ))?
            .to_string();
        let new_scores = body.get("newScores").ok_or(AppError::new(
            StatusCode::BAD_REQUEST,
            String::from("Missing new scores."),
        ))?;
        let new_scores = serde_json::from_value(new_scores.clone())?;
        let new_gp_scores = body.get("newGPScores").ok_or(AppError::new(
            StatusCode::BAD_REQUEST,
            String::from("Missing new GP scores."),
        ))?;
        let new_gp_scores = serde_json::from_value(new_gp_scores.clone())?;

        team.number = new_team_number.to_string();
        team.name = new_team_name;
        team.scores = new_scores;
        team.gp_scores = new_gp_scores;
        Ok((StatusCode::OK, "Team edits saved."))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::app_state::create_new_shared_state;
    use axum::body::Body;
    use axum::http::{Request, StatusCode, header};
    use http_body_util::BodyExt;
    use serde_json::{Value, json};
    use tower::{Service, ServiceExt};

    #[tokio::test]
    async fn defaults_to_no_teams() {
        let app_state = create_new_shared_state();
        let app = TeamHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/team/all")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(body, json!(Vec::<Team>::new()));
    }

    #[tokio::test]
    async fn get_teams() {
        let app_state = create_new_shared_state();
        app_state.lock().unwrap().teams = vec![
            Team::new(String::from("1234"), String::from("Test Team 1")),
            Team::new(String::from("5678"), String::from("Test Team 2")),
        ];
        let app = TeamHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/team/all")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(
            body,
            json!([{
                "number": "1234",
                "name": "Test Team 1",
                "scores": [],
                "gpScores": [],
                "displayScore": 0,
                "rank": 0
            }, {
                "number": "5678",
                "name": "Test Team 2",
                "scores": [],
                "gpScores": [],
                "displayScore": 0,
                "rank": 0
            }])
        );
    }

    #[tokio::test]
    async fn get_specific_team() {
        let app_state = create_new_shared_state();
        app_state.lock().unwrap().teams = vec![
            Team::new(String::from("1234"), String::from("Test Team 1")),
            Team::new(String::from("5678"), String::from("Test Team 2")),
        ];
        let app = TeamHandler::register_routes().with_state(app_state);

        let req = Request::builder()
            .method("GET")
            .uri("/team/5678")
            .body(Body::empty())
            .unwrap();
        let response = app.oneshot(req).await.unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let body = response.into_body().collect().await.unwrap().to_bytes();
        let body: Value = serde_json::from_slice(&body).unwrap();
        assert_eq!(
            body,
            json!({
                "number": "5678",
                "name": "Test Team 2",
                "scores": [],
                "gpScores": [],
                "displayScore": 0,
                "rank": 0
            })
        );
    }

    #[tokio::test]
    async fn add_team() {
        let app_state = create_new_shared_state();
        let mut app = TeamHandler::register_routes()
            .with_state(app_state.clone())
            .into_service();

        let req = Request::builder()
            .method("PUT")
            .uri("/team/add")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"number": "1234", "name": "Test Team"}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        let team = app_state.lock().unwrap().teams.first().unwrap().clone();
        assert_eq!(team.number, "1234");
        assert_eq!(team.name, "Test Team");

        let req = Request::builder()
            .method("PUT")
            .uri("/team/add")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"number": "", "name": "Empty number"}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::BAD_REQUEST);

        let req = Request::builder()
            .method("PUT")
            .uri("/team/add")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(
                r#"{"number": "1234", "name": "Duplicate number"}"#,
            ))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::BAD_REQUEST);
    }

    #[tokio::test]
    async fn edit_team() {
        let app_state = create_new_shared_state();
        app_state.lock().unwrap().teams = vec![
            Team::new(String::from("1234"), String::from("Test Team 1")),
            Team::new(String::from("5678"), String::from("Test Team 2")),
        ];
        let mut app = TeamHandler::register_routes()
            .with_state(app_state.clone())
            .into_service();

        let req = Request::builder()
            .method("PUT")
            .uri("/team/edit")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"oldTeamNumber": "1234", "newTeamNumber": "1234", "name": "Test Team", "newScores": [], "newGPScores": []}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);

        let req = Request::builder()
            .method("PUT")
            .uri("/team/edit")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"oldTeamNumber": "1234", "newTeamNumber": "", "name": "Test Team", "newScores": [], "newGPScores": []}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::BAD_REQUEST);

        let req = Request::builder()
            .method("PUT")
            .uri("/team/edit")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"oldTeamNumber": "1234", "newTeamNumber": "5678", "name": "Test Team", "newScores": [], "newGPScores": []}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::BAD_REQUEST);
    }

    #[tokio::test]
    async fn remove_team() {
        let app_state = create_new_shared_state();
        app_state.lock().unwrap().teams =
            vec![Team::new(String::from("1234"), String::from("Test Team"))];
        let mut app = TeamHandler::register_routes()
            .with_state(app_state.clone())
            .into_service();

        let req = Request::builder()
            .method("PUT")
            .uri("/team/remove")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"number": "1234"}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::OK);
        assert!(app_state.lock().unwrap().teams.is_empty());

        app_state.lock().unwrap().teams =
            vec![Team::new(String::from("1234"), String::from("Test Team"))];

        let req = Request::builder()
            .method("PUT")
            .uri("/team/add")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"number": ""}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::BAD_REQUEST);

        let req = Request::builder()
            .method("PUT")
            .uri("/team/add")
            .header(header::CONTENT_TYPE, "application/json")
            .body(Body::from(r#"{"number": "nonexistent"}"#))
            .unwrap();
        let response = ServiceExt::<Request<Body>>::ready(&mut app)
            .await
            .unwrap()
            .call(req)
            .await
            .unwrap();
        assert_eq!(response.status(), StatusCode::BAD_REQUEST);
    }
}
