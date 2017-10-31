#ifndef OLYMPUS_PHASE_H
#define OLYMPUS_PHASE_H

#include <vector>
#include <set>
#include "Match.h"
#include "../json.hpp"

struct Phase {
    std::vector<Match> matches;
    std::string name;

    nlohmann::json toJSON() const;

    std::set<std::string> getInvolvedTeamNumbers() const;
};

#endif //OLYMPUS_PHASE_H
