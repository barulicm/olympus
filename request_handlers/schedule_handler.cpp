#include "schedule_handler.h"

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
            if(session_.schedule.IsValid()) {
                auto match = session_.schedule.phases[session_.schedule.current_phase].matches_[session_.schedule.current_match];
                auto matchJson = match.ToJson();
                matchJson["number"] = session_.schedule.current_match;
                matchJson["phase"] = session_.schedule.GetCurrentPhase().name_;
                request.reply(web::http::status_codes::OK, matchJson.dump(), U("application/json")).wait();
            } else {
                request.reply(web::http::status_codes::OK, R"rawdelim({"ERROR":"No Matches Scheduled"})rawdelim", U("application/json")).wait();
            }
        }
    } else if(sched_request.substr(0,4) == "full") {
        auto schedule_json = session_.schedule.ToJson();
        request.reply(web::http::status_codes::OK, schedule_json.dump(), U("application/json")).wait();
    } else {
        request.reply(web::http::status_codes::NotFound, U("Unrecognized schedule request")).wait();
    }
}

void ScheduleHandler::CallbackPut(web::http::http_request request) {
    if(request.relative_uri().path() == "/schedule/load") {
        request.extract_string().then([this,&request](utility::string_t body){
            try {
                session_.schedule.phases.clear();

                nlohmann::json j = nlohmann::json::parse(body);

                auto phases = j["phases"];

                for(const auto &phase : phases) {
                    session_.schedule.phases.emplace_back();
                    session_.schedule.phases.back().name_ = phase["name_"];
                    auto matches = phase["matches"];
                    for(const auto &match : matches) {
                        session_.schedule.phases.back().matches_.emplace_back();
                        auto teams = match["teams"];
                        for(const auto &team : teams) {
                            session_.schedule.phases.back().matches_.back().team_numbers_.push_back(team);
                        }
                    }
                }
                session_.schedule.current_phase = 0;
                session_.schedule.current_match = 0;

                for(auto &team : session_.teams) {
                    team.scores_.resize(session_.schedule.phases.size());
                }

                session_.results.phase_results.resize(session_.schedule.phases.size());

                for(auto i = 0; i < session_.schedule.phases.size(); i++) {
                    session_.results.phase_results[i].phase_name = session_.schedule.phases[i].name_;
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
            if (session_.schedule.current_match < session_.schedule.GetCurrentPhase().matches_.size() - 1) {
                session_.schedule.current_match++;
                request.reply(web::http::status_codes::OK, U("Next Match")).wait();
            } else if (session_.schedule.current_phase < session_.schedule.phases.size() - 1) {
                FillNextPhase();
                session_.schedule.current_phase++;
                session_.schedule.current_match = 0;
                auto nextPhaseTeamNumbers = session_.schedule.GetCurrentPhase().GetInvolvedTeamNumbers();
                for(auto &team : session_.teams) {
                    auto find_iter = std::find(nextPhaseTeamNumbers.begin(), nextPhaseTeamNumbers.end(), team.number_);
                    if(find_iter != nextPhaseTeamNumbers.end()) {
                        team.rank_ = 0;
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
    auto currentPhase = session_.schedule.current_phase;
    std::set<std::string> nextPhaseTeamKeys;
    auto nextPhase = session_.schedule.phases[currentPhase + 1];

    for (const auto &match : nextPhase.matches_) {
        for (const auto &team : match.team_numbers_) {
            nextPhaseTeamKeys.insert(team);
        }
    }

    nlohmann::json nextPhaseMap = nlohmann::json::array();

    for (const auto &teamKey : nextPhaseTeamKeys) {
        nextPhaseMap.push_back({teamKey, ""});
    }

    auto results = session_.results.ToJson();

    auto teamsJson = nlohmann::json::array();
    transform(session_.teams.begin(), session_.teams.end(), back_inserter(teamsJson),
              [](const auto &team) { return team.ToJson(); });

    auto ret = js_.callFunction("selectNextPhase", {results, teamsJson, currentPhase, nextPhaseMap});

    std::map<std::string, std::string> nextPhaseReplacementMap;

    for(const auto &retPair : ret) {
        nextPhaseReplacementMap[retPair[0]] = retPair[1];
    }

    for(auto &match : session_.schedule.phases[session_.schedule.current_phase + 1].matches_) {
        for(auto &teamNumber : match.team_numbers_) {
            teamNumber = nextPhaseReplacementMap[teamNumber];
        }
    }
}
