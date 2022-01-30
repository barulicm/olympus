#include "team.h"

using namespace std;

Team::Team() {
    custom_fields_ = nlohmann::json();
}

nlohmann::json Team::ToJson() const {
    nlohmann::json j = {
            {"number",       number_},
            {"name",        name_},
            {"rank",        rank_},
            {"displayScore", display_score_}
    };
    j["scores"] = nlohmann::json::array();
    for(const auto &phase : scores_) {
        auto phase_scores = nlohmann::json::array();
        copy(phase.begin(), phase.end(), back_inserter(phase_scores));
        j["scores"].push_back(phase_scores);
    }
    j["customFields"] = custom_fields_;
    return j;
}
