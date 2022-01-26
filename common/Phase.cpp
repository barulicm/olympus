#include "Phase.h"

nlohmann::json Phase::toJSON() const {
    nlohmann::json j {
            {"name", name}
    };
    j["matches"] = nlohmann::json::array();
    int matchNumber = 1;
    for(const auto &match : matches) {
        auto matchJSON = match.toJSON();
        matchJSON["number"] = matchNumber;
        matchNumber++;
        j["matches"].push_back(matchJSON);
    }
    return j;
}

std::set<std::string> Phase::getInvolvedTeamNumbers() const {
    std::set<std::string> team_numbers;

    for(const auto &match : matches) {
        for(const auto &team_number : match.team_numbers) {
            team_numbers.insert(team_number);
        }
    }

    return team_numbers;
}
