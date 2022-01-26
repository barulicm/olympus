#include "Match.h"

nlohmann::json Match::toJSON() const {
    nlohmann::json j;
    j["teams"] = nlohmann::json::array();
    for(const auto &team_number : team_numbers) {
        j["teams"].push_back(team_number);
    }
    return j;
}
