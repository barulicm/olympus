#include "phase.h"

nlohmann::json Phase::ToJson() const {
    nlohmann::json j {
            {"name_", name_}
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

std::set<std::string> Phase::GetInvolvedTeamNumbers() const {
    std::set<std::string> team_numbers;

    for(const auto &match : matches_) {
        for(const auto &team_number : match.team_numbers_) {
            team_numbers.insert(team_number);
        }
    }

    return team_numbers;
}
