#ifndef OLYMPUS_MATCH_H
#define OLYMPUS_MATCH_H

#include <vector>
#include "Team.h"
#include "../json.hpp"

struct Match {

    std::vector<std::string> team_numbers;

    // TODO colors?

    nlohmann::json toJSON() const;

};


#endif //OLYMPUS_MATCH_H
