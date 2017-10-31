#include "PhaseResults.h"

nlohmann::json PhaseResults::toJSON() const {
   auto j = nlohmann::json::array();

    for(const auto &phase : rankings) {
        auto j_phase = nlohmann::json::array();
        for(const auto &ranking : phase) {
            j_phase.push_back({ranking.rank,ranking.teamNumber});
        }
        j.push_back(j_phase);
    }

    return j;
}
