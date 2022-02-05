#ifndef OLYMPUS_MATCH_H
#define OLYMPUS_MATCH_H

#include <vector>
#include "team.h"
#include <nlohmann/json.hpp>

struct Match {

    std::vector<std::string> team_numbers_;

    // TODO colors?

    [[nodiscard]] nlohmann::json ToJson() const;
    static Match FromJson(const nlohmann::json& json);

};

#endif //OLYMPUS_MATCH_H
