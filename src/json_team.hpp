#ifndef JSON_TEAM_HPP_
#define JSON_TEAM_HPP_

#include "team.hpp"
#include <nlohmann/json.hpp>

void to_json(nlohmann::json& json, const Team& team);
void from_json(const nlohmann::json& json, Team& team);

#endif //JSON_TEAM_HPP_
