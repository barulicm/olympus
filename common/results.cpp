#include "results.h"

nlohmann::json PhaseResults::ToJson() const {

    nlohmann::json j;

    j["name_"] = phase_name;

    j["rankings"] = nlohmann::json::array();

    for(const auto &ranking : rankings) {
        j["rankings"].push_back({ranking.rank, ranking.team_number});
    }

    return j;
}

nlohmann::json TournamentResults::ToJson() const {
    nlohmann::json j;

    j["phases"] = nlohmann::json::array();

    for(const auto &phase : phase_results) {
        j["phases"].push_back(phase.ToJson());
    }

    return j;
}
