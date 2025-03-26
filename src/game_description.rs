use serde::{Deserialize, Serialize};
use std::io;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GameDescription {
    pub name: String,
    pub description: String,
    pub logo: String,
    pub missions: Vec<MissionDescription>,
}

impl GameDescription {
    pub fn from_path(path: &std::path::PathBuf) -> io::Result<Self> {
        let file_contents = std::fs::read_to_string(path)?;
        let json_value = serde_json::from_str(file_contents.as_str())?;
        Ok(json_value)
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct MissionDescription {
    pub name: String,
    #[serde(rename="scoring")]
    pub scoring_expression: String,
    #[serde(rename="validation")]
    #[serde(default="generate_default_validation_expresion")]
    pub validation_expression: String,
    pub questions: Vec<QuestionDescription>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(tag = "type", rename_all = "lowercase")]
pub enum QuestionDescription {
    Bool {
        description: String,
    },
    Enum {
        description: String,
        options: Vec<String>,
    },
    Number {
        description: String,
        min: i64,
        max: i64,
        #[serde(default="generate_default_show_plus_on_max")]
        show_plus_on_max: bool,
    },
}

fn generate_default_validation_expresion() -> String {
  "true".to_string()
}

fn generate_default_show_plus_on_max() -> bool {
    false
}
