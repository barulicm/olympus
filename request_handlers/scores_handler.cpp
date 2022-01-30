#include "scores_handler.h"

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

                std::string teamNumber = j["team_number"];
                int score = j["score"];

                auto team_iter = find_if(session_.teams.begin(), session_.teams.end(),
                                         [&teamNumber](const Team &t) {
                                             return t.number_ == teamNumber;
                                         });
                if(team_iter != session_.teams.end()) {
                    team_iter->scores_[session_.schedule.current_phase].push_back(score);

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
    std::for_each(session_.teams.begin(), session_.teams.end(),
                  [this](Team &team){
                 nlohmann::json response = js_.callFunction("GetTeamScore",{team.ToJson(), session_.schedule.current_phase});
                 team.display_score_ = response["score"];
             });
    std::stable_sort(session_.teams.begin(), session_.teams.end(),
                     [this](const Team &a, const Team &b){
                    nlohmann::json response = js_.callFunction("CompareTeams",{a.ToJson(), b.ToJson()});
                    return response["result"];
                });
    int rank = 1;
    session_.teams[0].rank_=rank;
    for(size_t i = 1; i < session_.teams.size(); i++) {
        if(js_.callFunction("CompareTeams",{session_.teams[i - 1].ToJson(), session_.teams[i].ToJson()})["result"]) {
            rank++;
        }
        session_.teams[i].rank_ = rank;
    }
}

void ScoresHandler::UpdateResults() {
    auto &currentPhaseResults = session_.results.phase_results[session_.schedule.current_phase];

    currentPhaseResults.rankings.clear();

    auto teamNumbers = session_.schedule.GetCurrentPhase().GetInvolvedTeamNumbers();

    auto ranking_from_team_number = [this](const auto& team_number){
        const auto& team = GetTeamByNumber(team_number);
        return Ranking{team.rank_, team.number_};
    };

    std::transform(teamNumbers.begin(),
                   teamNumbers.end(),
                   std::back_inserter(currentPhaseResults.rankings),
                   ranking_from_team_number);
}

Team &ScoresHandler::GetTeamByNumber(const std::string &number) {
    const auto found_iter = std::find_if(session_.teams.begin(), session_.teams.end(), [&number](const Team& team){
        return team.number_ == number;
    });
    if(found_iter == session_.teams.end()) {
        throw std::invalid_argument("No team found with number: " + number);
    }
    return *found_iter;
}