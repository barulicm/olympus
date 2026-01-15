use crate::game_description::GameDescription;
use serde::{Deserialize, Serialize};
use std::sync::{Arc, Mutex};
use strum_macros::Display;

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
    #[serde(skip)]
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
    pub display_state: DisplayState,
    pub display_seconds_per_page: u64,
}

impl Default for Config {
    fn default() -> Self {
        Config {
            show_timer: false,
            competition_name: "FLL Competition".to_string(),
            display_state: DisplayState::ShowScores,
            display_seconds_per_page: 10,
        }
    }
}

#[derive(Display, Debug, Serialize, Deserialize, PartialEq)]
pub enum DisplayState {
    ShowScores,
    Blackout,
    FllLogo,
    Sponsors,
}

#[derive(Clone, Serialize, Deserialize)]
pub struct Team {
    pub number: String,
    pub name: String,
    pub scores: Vec<i64>,
    #[serde(rename = "gpScores")]
    pub gp_scores: Vec<i64>,
    #[serde(rename = "displayScore")]
    pub display_score: i64,
    pub rank: i64,
}

impl Team {
    pub fn new(number: String, name: String) -> Self {
        Self {
            number,
            name,
            scores: vec![],
            gp_scores: vec![],
            display_score: 0,
            rank: 0,
        }
    }
}
