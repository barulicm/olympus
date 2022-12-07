#ifndef TEAM_HPP_
#define TEAM_HPP_

#include <string>
#include <vector>

struct Team
{
    std::string number;
    std::string name;
    std::vector<int> scores;
    int display_score=0;
    int rank=0;
};

#endif //TEAM_HPP_
