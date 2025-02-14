
#include "json_game.hpp"
#include "json_mission.hpp"

namespace olympus::game_description {

void to_json(nlohmann::json &json, const Game &game) {
    json = {
            {"name",      game.name},
            {"description", game.description},
            {"logo", game.logo_path},
            {"missions",  game.missions}
    };
}

void from_json(const nlohmann::json &json, Game &game) {
    json.at("name").get_to(game.name);
    json.at("description").get_to(game.description);
    json.at("logo").get_to(game.logo_path);
    json.at("missions").get_to(game.missions);
}

}
