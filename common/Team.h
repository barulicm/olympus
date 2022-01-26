#ifndef OLYMPUS_TEAM_H
#define OLYMPUS_TEAM_H

#include <string>
#include <vector>
#include "json.hpp"

class Team {
public:

    Team();

    int rank;
    std::string number;
    std::string name;
    double displayScore = 0.0;
    std::vector<std::vector<int>> scores;

    nlohmann::json customFields;

    nlohmann::json toJSON() const;

};


#endif //OLYMPUS_TEAM_H
