use tinytemplate::TinyTemplate;

use serde::Serialize;
use crate::game_description::{GameDescription, QuestionDescription};

#[derive(Serialize)]
struct QuestionContext {
    id: String,
    in_mission_id: String,
    is_bool: bool,
    is_enum: bool,
    is_number: bool,
    options: Vec<String>,
    show_plus_on_max: bool,
}

#[derive(Serialize)]
struct MissionContext {
    id: String,
    name: String,
    scoring_expression: String,
    validation_expression: String,
    questions: Vec<QuestionContext>,
}

#[derive(Serialize)]
struct Context {
    missions: Vec<MissionContext>,
    question_ids: Vec<String>,
}

pub fn render_scorecard(game_description: &GameDescription) -> Result<String, tinytemplate::error::Error> {
    let mut tt = TinyTemplate::new();
    tt.add_template("scorecard", include_str!("templates/scorecard.html.template"))?;
    tt.render("scorecard", &build_context(game_description.clone()))
}

pub fn render_score_calculator(game_description: &GameDescription) -> Result<String, tinytemplate::error::Error> {
  let mut tt = TinyTemplate::new();
  tt.add_template("calculator", include_str!("templates/score_calculator.js.template"))?;
  tt.render("calculator", &build_context(game_description.clone()))
}

fn format_question_in_mission_id(question_index: usize) -> String {
    format!("{}", char::from_u32(('a' as u32) + question_index as u32).unwrap())
}

fn format_question_id(mission_index: usize, question_index: usize) -> String {
    format!("M{}{}", mission_index, format_question_in_mission_id(question_index))
}

fn build_context(game_description: GameDescription) -> Context {
  Context {
    missions: game_description.missions.iter().enumerate().map(|(mission_index, mission)| {
      MissionContext {
        id: format!("M{}", mission_index),
        name: mission.name.clone(),
        scoring_expression: mission.scoring_expression.clone(),
        validation_expression: mission.validation_expression.clone(),
        questions: mission.questions.iter().enumerate().map(|(question_index, question)| {
          QuestionContext {
            id: format_question_id(mission_index, question_index),
            in_mission_id: format_question_in_mission_id(question_index),
            is_bool: matches!(question, QuestionDescription::Bool { .. }),
            is_enum: matches!(question, QuestionDescription::Enum { .. }),
            is_number: matches!(question, QuestionDescription::Number { .. }),
            options: if let QuestionDescription::Enum { options, .. } = question {
              options.clone()
            } else if let QuestionDescription::Number { min, max, .. } = question {
              (*min..=*max).map(|n| n.to_string()).collect()
            } else {
              vec![]
            },
            show_plus_on_max: if let QuestionDescription::Number { show_plus_on_max, .. } = question {
              *show_plus_on_max
            } else {
              false
            },
          }
        }).collect(),
      }
    }).collect(),
    question_ids: game_description.missions.iter().enumerate().flat_map(|(mission_index, mission)| {
      mission.questions.iter().enumerate().map(|(question_index, _question)| {
        format_question_id(mission_index, question_index)
      }).collect::<Vec<_>>()
    }).collect(),
  }
}
