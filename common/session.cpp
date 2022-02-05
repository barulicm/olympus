#include "session.h"

nlohmann::json Session::ToJson() const {
    nlohmann::json json_rep;

    json_rep["teams"] = nlohmann::json::array();
    std::transform(teams.begin(), teams.end(), std::back_inserter(json_rep["teams"]), std::mem_fn(&Team::ToJson));

    json_rep["schedule"] = schedule.ToJson();

    json_rep["results"] = results.ToJson();

    json_rep["config"] = config.ToJson();

    return json_rep;
}

Session Session::FromJson(const nlohmann::json &json) {
    Session session;

    const auto& json_teams = json["teams"];
    std::transform(json_teams.begin(), json_teams.end(), std::back_inserter(session.teams), &Team::FromJson);

    session.schedule = Schedule::FromJson(json["schedule"]);
    session.results = TournamentResults::FromJson(json["results"]);
    session.config = Config::FromJson(json["config"]);

    return session;
}

