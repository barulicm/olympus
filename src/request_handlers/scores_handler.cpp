#include "scores_handler.hpp"
#include <nlohmann/json.hpp>

ScoresHandler::ScoresHandler(Session &session)
: session_(session)
{

}

std::vector<RequestHandlerDetails> ScoresHandler::GetHandlers() {
    return {
            {
                web::http::methods::PUT,
                [](const auto& path){ return path.starts_with("/scores/");},
                std::bind_front(&ScoresHandler::CallbackPut, this)
            },
            {
                web::http::methods::GET,
                [](const auto& path){ return path.starts_with("/scores/");},
                std::bind_front(&ScoresHandler::CallbackGet, this)
            }
    };
}

void ScoresHandler::CallbackPut(web::http::http_request request) {
    if(request.relative_uri().path() == "/scores/submit") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);

                std::string teamNumber = j["teamNumber"];
                int score = j["score"];

                auto team_iter = find_if(session_.teams.begin(), session_.teams.end(),
                                         [&teamNumber](const Team &t) {
                                             return t.number == teamNumber;
                                         });
                if(team_iter != session_.teams.end()) {
                    team_iter->scores.push_back(score);

                    UpdateRanks();

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

void ScoresHandler::CallbackGet(web::http::http_request request) {
    try {
        std::stringstream csv_content;
        auto score_count = 0ul;
        for(const auto& team : session_.teams) {
            score_count = std::max(score_count, team.scores.size());
        }
        csv_content << "rank,team number,team name";
        for(auto i = 0ul; i < score_count; ++i) {
            csv_content << ",match " << (i+1);
        }
        csv_content << ",final score\n";
        for (const auto &team: session_.teams) {
            csv_content << team.rank << "," << team.number << "," << team.name;
            for (const auto &score: team.scores) {
                csv_content << "," << score;
            }
            for(auto i = 0ul; i < (score_count - team.scores.size()); ++i) {
                csv_content << ",0";
            }
            csv_content << "," << team.display_score;
            csv_content << "\n";
        }
        request.reply(web::http::status_codes::OK, csv_content.str(), U("text/csv")).wait();
    } catch(const std::exception &e) {
        std::string rep = U("Exporting scores failed.");
        request.reply(web::http::status_codes::InternalError, rep).wait();
        std::cerr << "Score export: Exception occurred:\n\t";
        std::cerr << e.what() << std::endl;
    }
}

void ScoresHandler::UpdateRanks() {
    std::for_each(session_.teams.begin(), session_.teams.end(), [](Team &team){
        if(team.scores.empty()) {
            team.display_score = 0;
            return;
        }
        team.display_score = *std::max_element(team.scores.begin(), team.scores.end());
    });
    auto greater_by_display_score = [](const Team &a, const Team &b) {
        return a.display_score > b.display_score;
    };
    std::stable_sort(session_.teams.begin(), session_.teams.end(), greater_by_display_score);
    int rank = 1;
    session_.teams[0].rank=rank;
    for(size_t i = 1; i < session_.teams.size(); i++) {
        if(greater_by_display_score(session_.teams[i-1], session_.teams[i])) {
            rank++;
        }
        session_.teams[i].rank = rank;
    }
}

Team &ScoresHandler::GetTeamByNumber(const std::string &number) {
    const auto found_iter = std::find_if(session_.teams.begin(), session_.teams.end(), [&number](const Team& team){
        return team.number == number;
    });
    if(found_iter == session_.teams.end()) {
        throw std::invalid_argument("No team found with number: " + number);
    }
    return *found_iter;
}
