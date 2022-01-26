#include "team.h"

using namespace std;

Team::Team() {
    custom_fields_ = nlohmann::json();
}

nlohmann::json Team::ToJson() const {
    nlohmann::json j = {
            {"number",       number_},
            {"name_",        name_},
            {"rank_",        rank_},
            {"display_score_", display_score_}
    };
    j["scores"] = nlohmann::json::array();
    for(const auto &phase : scores_) {
        auto phase_scores = nlohmann::json::array();
        copy(phase.begin(), phase.end(), back_inserter(phase_scores));
        j["scores"].push_back(phase_scores);
    }
    j["custom_fields_"] = custom_fields_;
    return j;
}
