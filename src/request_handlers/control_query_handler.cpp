#include "control_query_handler.hpp"

ControlQueryHandler::ControlQueryHandler(Session &session)
    : session_(session)
{

}

std::vector<RequestHandlerDetails> ControlQueryHandler::GetHandlers() {
    return {
            {
                web::http::methods::GET,
                [](const auto& path){ return path == U("/controlQuery");},
                std::bind_front(&ControlQueryHandler::Callback, this)
            }
    };
}

void ControlQueryHandler::Callback(const web::http::http_request &request) {
    const auto query_header_iter = request.headers().find(U("query"));
    if(query_header_iter == request.headers().end()) {
        request.reply(web::http::status_codes::BadRequest, U("Missing required header: query"), U("text/plain")).wait();
        return;
    }
    auto query = query_header_iter->second;
    utility::string_t response;
    if(query == U("hasTeams")) {
        response = (!session_.teams.empty() ? U("true") : U("false"));
    } else if(query == U("hasMissions")) {
        response = (!session_.missions.empty() ? U("true") : U("false"));
    }
    request.reply(web::http::status_codes::OK, response, U("text/plain")).wait();
}
