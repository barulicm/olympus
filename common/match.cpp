#include "match.h"

nlohmann::json Match::ToJson() const {
    nlohmann::json j;
    j["teams"] = nlohmann::json::array();
    for (const auto &team_number: team_numbers_) {
        j["teams"].push_back(team_number);
    }
    return j;
}
