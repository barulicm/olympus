#include "json_team.hpp"

void to_json(nlohmann::json& json, const Team& team){
    json = {
            {"number", team.number},
            {"name", team.name},
            {"scores", team.scores},
            {"displayScore", team.display_score},
            {"rank", team.rank}
    };
}

void from_json(const nlohmann::json& json, Team& team){
    json.at("number").get_to(team.number);
    json.at("name").get_to(team.name);
    json.at("scores").get_to(team.scores);
    json.at("displayScore").get_to(team.display_score);
    json.at("rank").get_to(team.rank);
}
