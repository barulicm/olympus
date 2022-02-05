#include "session_save_handler.h"

SessionSaveHandler::SessionSaveHandler(Session &session)
    : session_(session)
{
}

std::vector<RequestHandlerDetails> SessionSaveHandler::GetHandlers() {
    return {
            {
                web::http::methods::GET,
                [](const auto& path) { return path == "/session_backup.json"; },
                std::bind_front(&SessionSaveHandler::Callback, this)
            }
    };
}

void SessionSaveHandler::Callback(const web::http::http_request &request) {
    const auto json = session_.ToJson();
    request.reply(web::http::status_codes::OK, U(json.dump()), U("application/json"));
}
