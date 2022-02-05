#ifndef OLYMPUS_PHASERESULTS_H
#define OLYMPUS_PHASERESULTS_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

struct Ranking {
    int rank;
    std::string team_number;
};

struct PhaseResults {
    std::string phase_name;
    std::vector<Ranking> rankings;

    [[nodiscard]] nlohmann::json ToJson() const;
    static PhaseResults FromJson(const nlohmann::json& json);
};

struct TournamentResults {
    std::vector<PhaseResults> phase_results;

    [[nodiscard]] nlohmann::json ToJson() const;
    static TournamentResults FromJson(const nlohmann::json& json);
};


#endif //OLYMPUS_PHASERESULTS_H
