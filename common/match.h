#ifndef OLYMPUS_MATCH_H
#define OLYMPUS_MATCH_H

#include <vector>
#include "team.h"
#include <nlohmann/json.hpp>

struct Match {

    std::vector<std::string> team_numbers_;

    // TODO colors?

    [[nodiscard]] nlohmann::json ToJson() const;

};

#endif //OLYMPUS_MATCH_H
