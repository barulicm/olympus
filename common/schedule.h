#ifndef OLYMPUS_SCHEDULE_H
#define OLYMPUS_SCHEDULE_H

#include <vector>
#include "phase.h"
#include <nlohmann/json.hpp>

struct Schedule {
    std::vector<Phase> phases;

    int current_phase = 0;

    int current_match = 0;

    Phase &GetCurrentPhase() {
        return phases[current_phase];
    }

    [[nodiscard]] const Phase &GetCurrentPhase() const {
        return phases[current_phase];
    }

    Match &GetCurrentMatch() {
        return phases[current_phase].matches_[current_match];
    }

    [[nodiscard]] const Match &GetCurrentMatch() const {
        return phases[current_phase].matches_[current_match];
    }

    [[nodiscard]] bool IsValid() const {
        return current_phase < phases.size() && current_match < phases[current_phase].matches_.size();
    }

    [[nodiscard]] nlohmann::json ToJson() const;
};


#endif //OLYMPUS_SCHEDULE_H
