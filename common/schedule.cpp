#include "schedule.h"

nlohmann::json Schedule::ToJson() const {
    nlohmann::json j {
            {"current_phase", current_phase},
            {"current_match", current_match}
    };
    j["phases"] = nlohmann::json::array();
    for(const auto &phase : phases) {
        j["phases"].push_back(phase.ToJson());
    }
    return j;
}
