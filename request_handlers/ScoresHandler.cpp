#include "ScoresHandler.h"

ScoresHandler::ScoresHandler(Session &session, JavascriptExecutor& js)
: session_(session),
  js_(js)
{

}

std::vector<RequestHandlerDetails> ScoresHandler::GetHandlers() {
    return {
            {
                    web::http::methods::PUT,
                    [](const auto& path){ return path.starts_with("/scores/");},
                    std::bind_front(&ScoresHandler::Callback, this)
            }
    };
}

void ScoresHandler::Callback(web::http::http_request request) {
    if(request.relative_uri().path() == "/scores/submit") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);

                std::string teamNumber = j["teamNumber"];
                int score = j["score"];

                auto team_iter = find_if(session_._teams.begin(),session_._teams.end(),
                                         [&teamNumber](const Team &t) {
                                             return t.number == teamNumber;
                                         });
                if(team_iter != session_._teams.end()) {
                    team_iter->scores[session_._schedule.currentPhase].push_back(score);

                    UpdateRanks();

                    UpdateResults();

                    std::string rep = U("Score submission successful.");
                    request.reply(web::http::status_codes::OK, rep).wait();
                } else {
                    std::string rep = U("Score submission failed. Nonexistent team number.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                    return;
                }
            } catch(const std::exception &e) {
                std::string rep = U("Score submission failed.");
                request.reply(web::http::status_codes::InternalError, rep).wait();
                std::cerr << "Score submission: Exception occurred:\n\t";
                std::cerr << e.what() << std::endl;
            }
        }).wait();
    } else if (request.relative_uri().path() == "/scores/rerank") {
        try {
            UpdateRanks();
            request.reply(web::http::status_codes::OK, U("Score rerank successful.")).wait();
        } catch(const std::exception &e) {
            std::string rep = U("Score rerank failed.");
            request.reply(web::http::status_codes::InternalError, rep).wait();
            std::cerr << "Score rerank: Exception occurred:\n\t";
            std::cerr << e.what() << std::endl;
        }
    } else {
        request.reply(web::http::status_codes::NotFound, U("Resource not found")).wait();
    }
}

void ScoresHandler::UpdateRanks() {
    std::for_each(session_._teams.begin(), session_._teams.end(),
             [this](Team &team){
                 nlohmann::json response = js_.callFunction("GetTeamScore",{team.toJSON(),session_._schedule.currentPhase});
                 team.displayScore = response["score"];
             });
    std::stable_sort(session_._teams.begin(), session_._teams.end(),
                [this](const Team &a, const Team &b){
                    nlohmann::json response = js_.callFunction("CompareTeams",{a.toJSON(),b.toJSON()});
                    return response["result"];
                });
    int rank = 1;
    session_._teams[0].rank=rank;
    for(size_t i = 1; i < session_._teams.size(); i++) {
        if(js_.callFunction("CompareTeams",{session_._teams[i-1].toJSON(),session_._teams[i].toJSON()})["result"]) {
            rank++;
        }
        session_._teams[i].rank = rank;
    }
}

void ScoresHandler::UpdateResults() {
    auto &currentPhaseResults = session_._results.phaseResults[session_._schedule.currentPhase];

    currentPhaseResults.rankings.clear();

    auto teamNumbers = session_._schedule.getCurrentPhase().getInvolvedTeamNumbers();

    auto ranking_from_team_number = [this](const auto& team_number){
        const auto& team = GetTeamByNumber(team_number);
        return Ranking{team.rank, team.number};
    };

    std::transform(teamNumbers.begin(),
                   teamNumbers.end(),
                   std::back_inserter(currentPhaseResults.rankings),
                   ranking_from_team_number);
}

Team &ScoresHandler::GetTeamByNumber(const std::string &number) {
    const auto found_iter = std::find_if(session_._teams.begin(), session_._teams.end(), [&number](const Team& team){
        return team.number == number;
    });
    if(found_iter == session_._teams.end()) {
        throw std::invalid_argument("No team found with number: " + number);
    }
    return *found_iter;
}
