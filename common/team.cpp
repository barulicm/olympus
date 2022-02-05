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

Team Team::FromJson(const nlohmann::json &json) {
    Team team;
    team.number_ = json["number"];
    team.name_ = json["name"];
    team.rank_ = json["rank"];
    team.display_score_ = json["displayScore"];
    team.custom_fields_ = json["customFields"];
    const auto& json_scores = json["scores"];
    for(const auto& json_phase_scores : json_scores) {
        team.scores_.emplace_back();
        std::copy(json_phase_scores.begin(), json_phase_scores.end(), std::back_inserter(team.scores_.back()));
    }
    return team;
}
