#ifndef OLYMPUS_PHASE_H
#define OLYMPUS_PHASE_H

#include <vector>
#include <set>
#include "match.h"
#include <nlohmann/json.hpp>

struct Phase {
    std::vector<Match> matches_;
    std::string name_;

    [[nodiscard]] nlohmann::json ToJson() const;

    [[nodiscard]] std::set<std::string> GetInvolvedTeamNumbers() const;
};

#endif //OLYMPUS_PHASE_H
