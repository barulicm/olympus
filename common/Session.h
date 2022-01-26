#ifndef OLYMPUS_SESSION_H
#define OLYMPUS_SESSION_H

#include <vector>
#include "Team.h"
#include "Schedule.h"
#include "Results.h"

struct Session {
    std::string _competition_name;
    std::vector<Team> _teams;
    Schedule _schedule;
    TournamentResults _results;
};

#endif //OLYMPUS_SESSION_H
