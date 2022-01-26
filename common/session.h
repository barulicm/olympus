#ifndef OLYMPUS_SESSION_H
#define OLYMPUS_SESSION_H

#include <vector>
#include "team.h"
#include "schedule.h"
#include "results.h"

struct Session {
    std::string competition_name;
    std::vector<Team> teams;
    Schedule schedule;
    TournamentResults results;
};

#endif //OLYMPUS_SESSION_H
