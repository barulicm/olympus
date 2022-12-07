#include "results_handler.hpp"
#include <nlohmann/json.hpp>

ResultsHandler::ResultsHandler(Session &session)
    : session_(session)
{

}

std::vector<RequestHandlerDetails> ResultsHandler::GetHandlers() {
    return {
            {
                    web::http::methods::GET,
                    [](const auto& path){ return path == "/results";},
                    std::bind_front(&ResultsHandler::Callback, this)
            }
    };
}

void ResultsHandler::Callback(const web::http::http_request &request) {
    nlohmann::json phase_results = {
            {"name", "Tournament"},
            {"rankings", nlohmann::json::array()}
    };
    auto& rankings_json = phase_results["rankings"];
    std::for_each(session_.teams.begin(), session_.teams.end(), [&rankings_json](const Team &team){
        rankings_json.push_back({team.rank, team.number});
    });
    nlohmann::json response;
    response["phases"] = nlohmann::json::array();
    response["phases"].push_back(phase_results);
//    auto resultsJson = session_.results.ToJson();
    request.reply(web::http::status_codes::OK, response.dump(), U("application/json")).wait();
//    request.reply(web::http::status_codes::NotImplemented, U("ResultsHandler::Callback not yet implemented"));
}
