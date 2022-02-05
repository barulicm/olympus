#include "results.h"

nlohmann::json PhaseResults::ToJson() const {

    nlohmann::json j;

    j["name"] = phase_name;

    j["rankings"] = nlohmann::json::array();

    for(const auto &ranking : rankings) {
        j["rankings"].push_back({ranking.rank, ranking.team_number});
    }

    return j;
}

PhaseResults PhaseResults::FromJson(const nlohmann::json &json) {
    PhaseResults results;
    results.phase_name = json["name"];
    for(const auto& ranking_json : json["rankings"]) {
        Ranking ranking;
        ranking.rank = ranking_json[0];
        ranking.team_number = ranking_json[1];
        results.rankings.push_back(ranking);
    }
    return results;
}

nlohmann::json TournamentResults::ToJson() const {
    nlohmann::json j;

    j["phases"] = nlohmann::json::array();

    for(const auto &phase : phase_results) {
        j["phases"].push_back(phase.ToJson());
    }

    return j;
}

TournamentResults TournamentResults::FromJson(const nlohmann::json &json) {
    TournamentResults results;
    const auto& json_phases = json["phases"];
    std::transform(json_phases.begin(), json_phases.end(), std::back_inserter(results.phase_results), &PhaseResults::FromJson);
    return results;
}
