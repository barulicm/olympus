#include "ScheduleHandler.h"

ScheduleHandler::ScheduleHandler(Session &session, JavascriptExecutor &javascript_executor)
    : session_(session),
      js_(javascript_executor)
{

}

std::vector<RequestHandlerDetails> ScheduleHandler::GetHandlers() {
    return {
            {
                    web::http::methods::PUT,
                    [](const auto& path){ return path.starts_with("/schedule/");},
                    std::bind_front(&ScheduleHandler::CallbackPut, this)
            },
            {
                    web::http::methods::GET,
                    [](const auto& path){ return path.starts_with("/schedule/");},
                    std::bind_front(&ScheduleHandler::CallbackGet, this)
            }
    };
}

void ScheduleHandler::CallbackGet(const web::http::http_request &request) {
    const auto path = request.relative_uri().path();
    auto sched_request = path.substr(10);
    if(sched_request.substr(0,6) == "match/") {
        auto match_number = sched_request.substr(6);
        if(match_number == "current") {
            if(session_._schedule.isValid()) {
                auto match = session_._schedule.phases[session_._schedule.currentPhase].matches[session_._schedule.currentMatch];
                auto matchJson = match.toJSON();
                matchJson["number"] = session_._schedule.currentMatch;
                matchJson["phase"] = session_._schedule.getCurrentPhase().name;
                request.reply(web::http::status_codes::OK, matchJson.dump(), U("application/json")).wait();
            } else {
                request.reply(web::http::status_codes::OK, R"rawdelim({"ERROR":"No Matches Scheduled"})rawdelim", U("application/json")).wait();
            }
        }
    } else if(sched_request.substr(0,4) == "full") {
        auto schedule_json = session_._schedule.toJSON();
        request.reply(web::http::status_codes::OK, schedule_json.dump(), U("application/json")).wait();
    } else {
        request.reply(web::http::status_codes::NotFound, U("Unrecognized schedule request")).wait();
    }
}

void ScheduleHandler::CallbackPut(web::http::http_request request) {
    if(request.relative_uri().path() == "/schedule/load") {
        request.extract_string().then([this,&request](utility::string_t body){
            try {
                session_._schedule.phases.clear();

                nlohmann::json j = nlohmann::json::parse(body);

                auto phases = j["phases"];

                for(const auto &phase : phases) {
                    session_._schedule.phases.emplace_back();
                    session_._schedule.phases.back().name = phase["name"];
                    auto matches = phase["matches"];
                    for(const auto &match : matches) {
                        session_._schedule.phases.back().matches.emplace_back();
                        auto teams = match["teams"];
                        for(const auto &team : teams) {
                            session_._schedule.phases.back().matches.back().team_numbers.push_back(team);
                        }
                    }
                }
                session_._schedule.currentPhase = 0;
                session_._schedule.currentMatch = 0;

                for(auto &team : session_._teams) {
                    team.scores.resize(session_._schedule.phases.size());
                }

                session_._results.phaseResults.resize(session_._schedule.phases.size());

                for(auto i = 0; i < session_._schedule.phases.size(); i++) {
                    session_._results.phaseResults[i].phaseName = session_._schedule.phases[i].name;
                }

                std::string rep = U("Schedule loading successful.");
                request.reply(web::http::status_codes::OK, rep).wait();
            } catch(const std::exception &e) {
                std::string rep = U("Schedule loading failed. ");
                rep += e.what();
                request.reply(web::http::status_codes::InternalError, rep).wait();
                std::cerr << e.what() << std::endl;
            }
        }).wait();
    } else if(request.relative_uri().path() == "/schedule/next") {
        try {
            if (session_._schedule.currentMatch < session_._schedule.getCurrentPhase().matches.size() - 1) {
                session_._schedule.currentMatch++;
                request.reply(web::http::status_codes::OK, U("Next Match")).wait();
            } else if (session_._schedule.currentPhase < session_._schedule.phases.size() - 1) {
                FillNextPhase();
                session_._schedule.currentPhase++;
                session_._schedule.currentMatch = 0;
                auto nextPhaseTeamNumbers = session_._schedule.getCurrentPhase().getInvolvedTeamNumbers();
                for(auto &team : session_._teams) {
                    auto find_iter = std::find(nextPhaseTeamNumbers.begin(), nextPhaseTeamNumbers.end(), team.number);
                    if(find_iter != nextPhaseTeamNumbers.end()) {
                        team.rank = 0;
                    }
                }
                request.reply(web::http::status_codes::OK, U("Next Phase")).wait();
            } else {
                request.reply(web::http::status_codes::InternalError, U("Tournament is over.")).wait();
            }
        } catch(std::exception &e) {
            request.reply(web::http::status_codes::InternalError, U(e.what())).wait();
        }
    } else {
        request.reply(web::http::status_codes::NotFound, U("Unrecognized schedule request")).wait();
    }
}

void ScheduleHandler::FillNextPhase() {
    auto currentPhase = session_._schedule.currentPhase;
    std::set<std::string> nextPhaseTeamKeys;
    auto nextPhase = session_._schedule.phases[currentPhase + 1];

    for (const auto &match : nextPhase.matches) {
        for (const auto &team : match.team_numbers) {
            nextPhaseTeamKeys.insert(team);
        }
    }

    nlohmann::json nextPhaseMap = nlohmann::json::array();

    for (const auto &teamKey : nextPhaseTeamKeys) {
        nextPhaseMap.push_back({teamKey, ""});
    }

    auto results = session_._results.toJSON();

    auto teamsJson = nlohmann::json::array();
    transform(session_._teams.begin(), session_._teams.end(), back_inserter(teamsJson),
              [](const auto &team) { return team.toJSON(); });

    auto ret = js_.callFunction("selectNextPhase", {results, teamsJson, currentPhase, nextPhaseMap});

    std::map<std::string, std::string> nextPhaseReplacementMap;

    for(const auto &retPair : ret) {
        nextPhaseReplacementMap[retPair[0]] = retPair[1];
    }

    for(auto &match : session_._schedule.phases[session_._schedule.currentPhase+1].matches) {
        for(auto &teamNumber : match.team_numbers) {
            teamNumber = nextPhaseReplacementMap[teamNumber];
        }
    }
}
