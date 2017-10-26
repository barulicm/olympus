#include "Phase.h"

json Phase::toJSON() const {
    json j {
            {"name", name}
    };
    j["matches"] = json::array();
    int matchNumber = 1;
    for(const auto &match : matches) {
        auto matchJSON = match.toJSON();
        matchJSON["number"] = matchNumber;
        matchNumber++;
        j["matches"].push_back(matchJSON);
    }
    return j;
}
