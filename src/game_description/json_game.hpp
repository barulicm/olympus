#ifndef OLYMPUS_JSON_GAME_HPP
#define OLYMPUS_JSON_GAME_HPP

#include "game_description/game.hpp"
#include <nlohmann/json.hpp>

namespace olympus::game_description {

void to_json(nlohmann::json& json, const Game& game);
void from_json(const nlohmann::json& json, Game& game);

}

#endif //OLYMPUS_JSON_GAME_HPP
