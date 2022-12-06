#include "session_save_handler.hpp"
#include "json_session.hpp"

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
    const nlohmann::json json = session_;
    request.reply(web::http::status_codes::OK, U(json.dump()), U("application/json"));
}
