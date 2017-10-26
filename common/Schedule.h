#ifndef OLYMPUS_SCHEDULE_H
#define OLYMPUS_SCHEDULE_H

#include <vector>
#include "Phase.h"
#include "../json.hpp"

struct Schedule {
    std::vector<Phase> phases;

    int currentPhase = 0;

    int currentMatch = 0;

    const Phase &getCurrentPhase() const {
        return phases[currentPhase];
    }

    const Match &getCurrentMatch() const {
        return phases[currentPhase].matches[currentMatch];
    }

    bool isValid() const {
        return currentPhase < phases.size() && currentMatch < phases[currentPhase].matches.size();
    }

    nlohmann::json toJSON() const;
};


#endif //OLYMPUS_SCHEDULE_H
