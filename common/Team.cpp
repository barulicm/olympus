#include "Team.h"

using namespace std;

Team::Team() {
    customFields = nlohmann::json();
}

nlohmann::json Team::toJSON() const {
    nlohmann::json j = {
            {"number",number},
            {"name",name},
            {"rank",rank},
            {"displayScore",displayScore}
    };
    j["scores"] = nlohmann::json::array();
    for(const auto &phase : scores) {
        auto phase_scores = nlohmann::json::array();
        copy(phase.begin(), phase.end(), back_inserter(phase_scores));
        j["scores"].push_back(phase_scores);
    }
    j["customFields"] = customFields;
    return j;
}
