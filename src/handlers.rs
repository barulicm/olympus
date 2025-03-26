mod config_handler;
mod control_query_handler;
mod game_description_handler;
mod generated_files_handler;
mod session_save_handler;
mod team_handler;

use axum::Router;
use crate::app_state::SharedAppState;

pub trait Handler {
    fn register_routes() -> Router::<SharedAppState>;
}


pub fn register_all_handlers() -> Router::<SharedAppState> {
  Router::<SharedAppState>::new()
    .merge(config_handler::ConfigHandler::register_routes())
    .merge(control_query_handler::ControlQueryHandler::register_routes())
    .merge(game_description_handler::GameDescriptionHandler::register_routes())
    .merge(session_save_handler::SessionSaveHandler::register_routes())
    .merge(team_handler::TeamHandler::register_routes())
    .merge(generated_files_handler::GeneratedFilesHandler::register_routes())
}
