#include "schedule.h"

nlohmann::json Schedule::ToJson() const {
    nlohmann::json j {
            {"current_phase", current_phase},
            {"current_match", current_match}
    };
    j["phases"] = nlohmann::json::array();
    std::transform(phases.begin(), phases.end(), std::back_inserter(j["phases"]), std::mem_fn(&Phase::ToJson));
    return j;
}

Schedule Schedule::FromJson(const nlohmann::json &json) {
    Schedule schedule;
    schedule.current_phase = json["current_phase"];
    schedule.current_match = json["current_match"];
    const auto& json_phases = json["phases"];
    std::transform(json_phases.begin(), json_phases.end(), std::back_inserter(schedule.phases), &Phase::FromJson);
    return schedule;
}
