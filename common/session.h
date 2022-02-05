#ifndef OLYMPUS_SESSION_H
#define OLYMPUS_SESSION_H

#include <vector>
#include <nlohmann/json.hpp>
#include "team.h"
#include "schedule.h"
#include "results.h"
#include "config.h"

struct Session {
    std::vector<Team> teams;
    Schedule schedule;
    TournamentResults results;
    Config config;

    nlohmann::json ToJson() const;
    static Session FromJson(const nlohmann::json& json);
};

#endif //OLYMPUS_SESSION_H
