#include "results_handler.hpp"

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
//    auto resultsJson = session_.results.ToJson();
//    request.reply(web::http::status_codes::OK, resultsJson.dump(), U("application/json")).wait();
    request.reply(web::http::status_codes::NotImplemented, U("ResultsHandler::Callback not yet implemented"));
}
