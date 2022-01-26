#include "ControlQueryHandler.h"

ControlQueryHandler::ControlQueryHandler(Session &session)
    : session_(session)
{

}

std::vector<RequestHandlerDetails> ControlQueryHandler::GetHandlers() {
    return {
            {
                web::http::methods::GET,
                [](const auto& path){ return path == "/controlQuery";},
                std::bind_front(&ControlQueryHandler::Callback, this)
            }
    };
}

void ControlQueryHandler::Callback(const web::http::http_request &request) {
    const auto query_header_iter = request.headers().find("query");
    if(query_header_iter == request.headers().end()) {
        request.reply(web::http::status_codes::BadRequest, U("Missing required header: query"), U("text/plain")).wait();
    }
    auto query = query_header_iter->second;
    std::string response;
    if(query == "hasTeams") {
        response = ( !session_._teams.empty() ? "true" : "false");
    } else if(query == "hasSchedule") {
        response = ( !session_._schedule.phases.empty() ? "true" : "false");
    } else if(query == "hasNextMatch") {
        response = ( !session_._schedule.phases.empty() && (session_._schedule.currentMatch < (session_._schedule.getCurrentPhase().matches.size()-1)) ? "true" : "false");
    } else if(query == "hasNextPhase") {
        response = ( session_._schedule.currentPhase < (session_._schedule.phases.size()-1) ? "true" : "false");
    }
    request.reply(web::http::status_codes::OK, response, U("text/plain")).wait();
}
