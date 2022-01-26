#ifndef OLYMPUS_PHASERESULTS_H
#define OLYMPUS_PHASERESULTS_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

struct Ranking {
    Ranking(int rank = 0, const std::string &teamNumber = "") {
        this->rank = rank;
        this->teamNumber = teamNumber;
    }
    int rank;
    std::string teamNumber;
};

struct PhaseResults {
    std::string phaseName;
    std::vector<Ranking> rankings;

    nlohmann::json toJSON() const;
};

struct TournamentResults {
    std::vector<PhaseResults> phaseResults;

    nlohmann::json toJSON() const;
};


#endif //OLYMPUS_PHASERESULTS_H
