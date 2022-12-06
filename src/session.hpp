#ifndef SESSION_HPP_
#define SESSION_HPP_

#include "mission.hpp"
#include "team.hpp"
#include "config.hpp"

struct Session
{
    Config config;
    std::vector<Mission> missions;
    std::vector<Team> teams;
};

#endif //SESSION_HPP_
