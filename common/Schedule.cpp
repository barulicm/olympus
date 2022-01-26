#include "Schedule.h"

nlohmann::json Schedule::toJSON() const {
    nlohmann::json j {
            {"currentPhase", currentPhase},
            {"currentMatch", currentMatch}
    };
    j["phases"] = nlohmann::json::array();
    for(const auto &phase : phases) {
        j["phases"].push_back(phase.toJSON());
    }
    return j;
}
