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
}

void from_json(const nlohmann::json& json, Config& config)
{
    json.at("show_timer").get_to(config.show_timer);
    json.at("competition_name").get_to(config.competition_name);
    json.at("rows_on_display").get_to(config.rows_on_display);
}
