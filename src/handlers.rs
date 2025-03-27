mod announcement_handler;
mod config_handler;
mod control_query_handler;
mod game_description_handler;
mod generated_files_handler;
mod scores_handler;
mod session_save_handler;
mod sponsors_handler;
mod team_handler;
mod timer_handler;

use crate::app_state::SharedAppState;
use axum::Router;

pub trait Handler {
    fn register_routes() -> Router<SharedAppState>;
}

pub fn register_all_handlers() -> Router<SharedAppState> {
    Router::<SharedAppState>::new()
        .merge(announcement_handler::AnnouncementHandler::register_routes())
        .merge(config_handler::ConfigHandler::register_routes())
        .merge(control_query_handler::ControlQueryHandler::register_routes())
        .merge(game_description_handler::GameDescriptionHandler::register_routes())
        .merge(scores_handler::ScoresHandler::register_routes())
        .merge(session_save_handler::SessionSaveHandler::register_routes())
        .merge(sponsors_handler::SponsorsHandler::register_routes())
        .merge(team_handler::TeamHandler::register_routes())
        .merge(timer_handler::TimerHandler::register_routes())
        .merge(generated_files_handler::GeneratedFilesHandler::register_routes())
}
