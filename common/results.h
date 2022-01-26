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
};

struct TournamentResults {
    std::vector<PhaseResults> phase_results;

    [[nodiscard]] nlohmann::json ToJson() const;
};


#endif //OLYMPUS_PHASERESULTS_H
