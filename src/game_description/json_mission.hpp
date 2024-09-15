#ifndef JSON_MISSION_DESCRIPTION_HPP_
#define JSON_MISSION_DESCRIPTION_HPP_

#include "mission.hpp"
#include <nlohmann/json.hpp>

namespace olympus::game_description {

void to_json(nlohmann::json &json, const Mission &mission);

void from_json(const nlohmann::json &json, Mission &mission);

void to_json(nlohmann::json &json, const Question &question);

void from_json(const nlohmann::json &json, Question &question);

void to_json(nlohmann::json &json, const BoolQuestion &question);

void from_json(const nlohmann::json &json, BoolQuestion &question);

void to_json(nlohmann::json &json, const NumberQuestion &question);

void from_json(const nlohmann::json &json, NumberQuestion &question);

void to_json(nlohmann::json &json, const EnumQuestion &question);

void from_json(const nlohmann::json &json, EnumQuestion &question);

}

#endif  // JSON_MISSION_DESCRIPTION_HPP_
