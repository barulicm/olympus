#ifndef OLYMPUS_TEAM_H
#define OLYMPUS_TEAM_H

#include <string>
#include <vector>
#include "../json.hpp"

using json = nlohmann::json;

class Team {
public:

    Team();

    int rank;
    std::string number;
    std::string name;
    double displayScore = 0.0;
    std::vector<std::vector<int>> scores;

    nlohmann::json customFields;

    json toJSON() const;

};


#endif //OLYMPUS_TEAM_H
