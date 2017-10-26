#ifndef OLYMPUS_PHASE_H
#define OLYMPUS_PHASE_H

#include <vector>
#include "Match.h"
#include "../json.hpp"

struct Phase {
    std::vector<Match> matches;
    std::string name;

    nlohmann::json toJSON() const;
};

#endif //OLYMPUS_PHASE_H
