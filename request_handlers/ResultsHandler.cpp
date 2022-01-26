#include "ResultsHandler.h"

ResultsHandler::ResultsHandler(Session &session)
    : session_(session)
{

}

std::vector<RequestHandlerDetails> ResultsHandler::GetHandlers() {
    return {
            {
                    web::http::methods::GET,
                    [](const auto& path){ return path.starts_with("/results/");},
                    std::bind_front(&ResultsHandler::Callback, this)
            }
    };
}

void ResultsHandler::Callback(const web::http::http_request &request) {
    auto resultsJson = session_._results.toJSON();
    request.reply(web::http::status_codes::OK, resultsJson.dump(), U("application/json")).wait();
}
