#include "phase.h"

nlohmann::json Phase::ToJson() const {
    nlohmann::json j {
            {"name", name_}
    };
    j["matches"] = nlohmann::json::array();
    int matchNumber = 1;
    for(const auto &match : matches_) {
        auto matchJSON = match.ToJson();
        matchJSON["number"] = matchNumber;
        matchNumber++;
        j["matches"].push_back(matchJSON);
    }
    return j;
}

Phase Phase::FromJson(const nlohmann::json &json) {
    Phase phase;
    phase.name_ = json["name"];
    const auto& json_matches = json["matches"];
    std::transform(json_matches.begin(), json_matches.end(), std::back_inserter(phase.matches_), &Match::FromJson);
    return phase;
}

std::set<std::string> Phase::GetInvolvedTeamNumbers() const {
    std::set<std::string> team_numbers;

    for(const auto &match : matches_) {
        for(const auto &team_number : match.team_numbers_) {
            team_numbers.insert(team_number);
        }
    }

    return team_numbers;
}
