#ifndef OLYMPUS_TEAM_H
#define OLYMPUS_TEAM_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class Team {
public:

    Team();

    int rank_;
    std::string number_;
    std::string name_;
    double display_score_ = 0.0;
    std::vector<std::vector<int>> scores_;

    nlohmann::json custom_fields_;

    [[nodiscard]] nlohmann::json ToJson() const;
    static Team FromJson(const nlohmann::json& json);
};


#endif //OLYMPUS_TEAM_H
