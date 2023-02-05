#include "json_session.hpp"
#include "json_mission.hpp"
#include "json_team.hpp"

void to_json(nlohmann::json& json, const Session& session){
    json = {
            {"missions", session.missions},
            {"teams", session.teams}
    };
}

void from_json(const nlohmann::json& json, Session& session){
    json.at("missions").get_to(session.missions);
    json.at("teams").get_to(session.teams);
}

void to_json(nlohmann::json& json, const Config& config)
{
    json = {
            {"show_timer", config.show_timer},
            {"competition_name", config.competition_name},
            {"rows_on_display", config.rows_on_display}
    };
    switch(config.display_state) {
        case Config::DisplayState::ShowScores:
            json["display_state"] = "ShowScores";
            break;
        case Config::DisplayState::Blackout:
            json["display_state"] = "Blackout";
            break;
        case Config::DisplayState::FllLogo:
            json["display_state"] = "FllLogo";
            break;
    }
}

void from_json(const nlohmann::json& json, Config& config)
{
    json.at("show_timer").get_to(config.show_timer);
    json.at("competition_name").get_to(config.competition_name);
    json.at("rows_on_display").get_to(config.rows_on_display);
    const auto display_state_json = json.at("display_state").get<std::string>();
    if(display_state_json == "ShowScores") {
        config.display_state = Config::DisplayState::ShowScores;
    } else if(display_state_json == "Blackout") {
        config.display_state = Config::DisplayState::Blackout;
    } else if(display_state_json == "FllLogo") {
        config.display_state = Config::DisplayState::FllLogo;
    }
}
