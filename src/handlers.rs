mod config_handler;
mod generated_files_handler;

use axum::Router;
use crate::app_state::SharedAppState;

pub trait Handler {
    fn register_routes() -> Router::<SharedAppState>;
}


pub fn register_all_handlers() -> Router::<SharedAppState> {
  Router::<SharedAppState>::new()
    .merge(config_handler::ConfigHandler::register_routes())
    .merge(generated_files_handler::GeneratedFilesHandler::register_routes())
}
