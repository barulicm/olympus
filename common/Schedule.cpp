#include "Schedule.h"

nlohmann::json Schedule::toJSON() const {
    json j {
            {"currentPhase", currentPhase},
            {"currentMatch", currentMatch}
    };
    j["phases"] = json::array();
    for(const auto &phase : phases) {
        j["phases"].push_back(phase.toJSON());
    }
    return j;
}
