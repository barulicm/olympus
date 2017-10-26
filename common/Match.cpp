#include "Match.h"

nlohmann::json Match::toJSON() const {
    json j;
    j["teams"] = json::array();
    for(const auto &team_number : team_numbers) {
        j["teams"].push_back(team_number);
    }
    return j;
}
