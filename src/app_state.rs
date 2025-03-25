use std::sync::{Arc, Mutex};
use serde::{Deserialize, Serialize};
use strum_macros::Display;
use crate::game_description::GameDescription;

pub type SharedAppState = Arc<Mutex<AppState>>;

pub fn create_new_shared_state() -> SharedAppState {
    Arc::new(Mutex::new(AppState::new()))
}

#[derive(Serialize, Deserialize)]
pub struct AppState {
  pub config: Config,
  pub game_description: Option<GameDescription>,
  pub teams: Vec<Team>,
  pub sponsor_logos: Vec<String>,
  pub resources_path: std::path::PathBuf,
}

impl AppState {
    pub fn new() -> Self {
        AppState {
            config: Config::default(),
            game_description: None,
            teams: Vec::new(),
            sponsor_logos: Vec::new(),
            resources_path: std::path::PathBuf::new(),
        }
    }
}

#[derive(Serialize, Deserialize)]
pub struct Config {
  pub show_timer: bool,
  pub competition_name: String,
  pub rows_on_display: usize,
  pub display_state: DisplayState,
  pub display_seconds_per_page: u64,
}

impl Default for Config {
    fn default() -> Self {
        Config {
            show_timer: false,
            competition_name: "FLL Competition".to_string(),
            rows_on_display: 14,
            display_state: DisplayState::ShowScores,
            display_seconds_per_page: 5,
        }
    }
}

#[derive(Display, Serialize, Deserialize)]
pub enum DisplayState {
  ShowScores,
  Blackout,
  FllLogo,
  Sponsors
}

#[derive(Serialize, Deserialize)]
pub struct Team {
  pub number: String,
  pub name: String,
  pub scores: Vec<i64>,
  pub gp_scores: Vec<i64>,
  pub display_score: i64,
  pub rank: i64,
}
