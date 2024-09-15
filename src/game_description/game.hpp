#ifndef OLYMPUS_GAME_HPP
#define OLYMPUS_GAME_HPP

#include <filesystem>
#include "mission.hpp"

namespace olympus::game_description {

struct Game {
    std::string name;
    std::string description;
    std::string logo_path;
    std::vector<Mission> missions;
};

}

#endif //OLYMPUS_GAME_HPP
