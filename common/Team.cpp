#include "Team.h"

using namespace std;

json Team::toJSON() const {
    json j = {
            {"number",number},
            {"name",name},
            {"rank",rank},
            {"displayScore",displayScore}
    };
    j["scores"] = json::array();
    for(const auto &phase : scores) {
        auto phase_scores = json::array();
        copy(phase.begin(), phase.end(), back_inserter(phase_scores));
        j["scores"].push_back(phase_scores);
    }
    return j;
}
