#include "Results.h"

nlohmann::json PhaseResults::toJSON() const {

    nlohmann::json j;

    j["name"] = phaseName;

    j["rankings"] = nlohmann::json::array();

    for(const auto &ranking : rankings) {
        j["rankings"].push_back({ranking.rank, ranking.teamNumber});
    }

    return j;
}

nlohmann::json TournamentResults::toJSON() const {
    nlohmann::json j;

    j["phases"] = nlohmann::json::array();

    for(const auto &phase : phaseResults) {
        j["phases"].push_back(phase.toJSON());
    }

    return j;
}
