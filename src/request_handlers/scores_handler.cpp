#include "scores_handler.hpp"
#include <ranges>
#include <nlohmann/json.hpp>

ScoresHandler::ScoresHandler(Session &session)
: session_(session)
{

}

std::vector<RequestHandlerDetails> ScoresHandler::GetHandlers() {
    return {
            {
                web::http::methods::PUT,
                [](const auto& path){ return path.starts_with(U("/scores/"));},
                std::bind_front(&ScoresHandler::CallbackPut, this)
            },
            {
                web::http::methods::GET,
                [](const auto& path){ return path.starts_with(U("/scores/export.csv"));},
                std::bind_front(&ScoresHandler::CallbackGetExport, this)
            },
            {
                web::http::methods::GET,
                [](const auto& path){ return path.starts_with(U("/scores/export_gp.csv"));},
                std::bind_front(&ScoresHandler::CallbackGetGPExport, this)
            }
    };
}

void ScoresHandler::CallbackPut(web::http::http_request request) {
    if(request.relative_uri().path() == U("/scores/submit")) {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);

                const std::string teamNumber = j["teamNumber"];

                const auto team_iter = find_if(session_.teams.begin(), session_.teams.end(),
                                         [&teamNumber](const Team &t) {
                                             return t.number == teamNumber;
                                         });

                if(team_iter == session_.teams.end()) {
                    utility::string_t rep = U("Score submission failed. Nonexistent team number.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                    return;
                }

                team_iter->scores.push_back(j["score"]);
                team_iter->gp_scores.push_back(j["gpScore"]);

                UpdateRanks();

                utility::string_t rep = U("Score submission successful.");
                request.reply(web::http::status_codes::OK, rep).wait();
            } catch(const std::exception &e) {
                utility::string_t rep = U("Score submission failed.");
                request.reply(web::http::status_codes::InternalError, rep).wait();
                std::cerr << "Score submission: Exception occurred:\n\t";
                std::cerr << e.what() << std::endl;
            }
        }).wait();
    } else if (request.relative_uri().path() == U("/scores/rerank")) {
        try {
            UpdateRanks();
            request.reply(web::http::status_codes::OK, U("Score rerank successful.")).wait();
        } catch(const std::exception &e) {
            utility::string_t rep = U("Score rerank failed.");
            request.reply(web::http::status_codes::InternalError, rep).wait();
            std::cerr << "Score rerank: Exception occurred:\n\t";
            std::cerr << e.what() << std::endl;
        }
    } else {
        request.reply(web::http::status_codes::NotFound, U("Resource not found")).wait();
    }
}

void ScoresHandler::CallbackGetExport(web::http::http_request request) {
    try {
        auto teams = session_.teams; // copying so we can sort locally
        // TODO(barulicm): should Team::number just be an int now?
        std::ranges::sort(teams, std::ranges::less(), [](const Team& t){ return std::stoi(t.number); });
        const auto score_count = std::ranges::max(teams | std::views::transform([](const auto& t){ return t.scores.size(); }));
        utility::stringstream_t csv_content;
        csv_content << "rank,team number,team name";
        for(auto i = 0ul; i < score_count; ++i) {
            csv_content << ",match " << (i+1);
        }
        csv_content << ",final score\n";
        for (const auto &team: teams) {
            csv_content << team.rank << U(",") << utility::string_t(team.number.begin(), team.number.end()) << U(",") << utility::string_t(team.name.begin(), team.name.end()) << U(",");
            std::ranges::copy(team.scores, std::ostream_iterator<int,utility::char_t>(csv_content, U(",")));
            std::ranges::fill_n(std::ostream_iterator<int,utility::char_t>(csv_content, U(",")), score_count - team.scores.size(), 0);
            csv_content << team.display_score;
            csv_content << "\n";
        }
        request.reply(web::http::status_codes::OK, csv_content.str(), U("text/csv")).wait();
    } catch(const std::exception &e) {
        utility::string_t rep = U("Exporting scores failed.");
        request.reply(web::http::status_codes::InternalError, rep).wait();
        std::cerr << "Score export: Exception occurred:\n\t";
        std::cerr << e.what() << std::endl;
    }
}


void ScoresHandler::CallbackGetGPExport(web::http::http_request request) {
    try {
        auto teams = session_.teams; // copying so we can sort locally
        std::ranges::sort(teams, std::ranges::less(), [](const Team& t){ return std::stoi(t.number); });
        const auto score_count = std::ranges::max(teams | std::views::transform([](const auto& t){ return t.gp_scores.size(); }));
        utility::stringstream_t csv_content;
        csv_content << "team number,team name";
        for(auto i = 0ul; i < score_count; ++i) {
            csv_content << ",match " << (i+1);
        }
        csv_content << ",total GP points\n";
        for (const auto &team: teams) {
            csv_content << utility::string_t(team.number.begin(),team.number.end()) << U(",") << utility::string_t(team.name.begin(),team.name.end()) << U(",");
            std::ranges::copy(team.gp_scores, std::ostream_iterator<int,utility::char_t>(csv_content, U(",")));
            std::ranges::fill_n(std::ostream_iterator<int,utility::char_t>(csv_content, U(",")), score_count - team.gp_scores.size(), 0);
            const auto total_gp_points = std::accumulate(team.gp_scores.begin(), team.gp_scores.end(), 0);
            csv_content << total_gp_points;
            csv_content << "\n";
        }
        request.reply(web::http::status_codes::OK, csv_content.str(), U("text/csv")).wait();
    } catch(const std::exception &e) {
        utility::string_t rep = U("Exporting scores failed.");
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
