#ifndef TEAM_HPP_
#define TEAM_HPP_

#include <string>
#include <vector>

struct Team
{
    std::string number;
    std::string name;
    std::vector<int> scores;

    // calculated fields
    int display_score;
    int rank;
};

#endif //TEAM_HPP_
