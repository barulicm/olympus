#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <optional>
#include "game_description/game.hpp"
#include "team.hpp"
#include "config.hpp"

struct Session
{
    Config config;
    std::optional<olympus::game_description::Game> game;
    std::vector<Team> teams;
    std::vector<std::string> sponsors;
};

#endif //SESSION_HPP_
