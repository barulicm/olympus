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
                std::bind_front(&SessionSaveHandler::ExportCallback, this)
            },
            {
                web::http::methods::PUT,
                [](const auto& path) { return path == "/session/import"; },
                std::bind_front(&SessionSaveHandler::ImportCallback, this)
            }
    };
}

void SessionSaveHandler::ExportCallback(const web::http::http_request &request) {
    const nlohmann::json json = session_;
    // Using MIME-type to force download instead of dumping JSON to the screen
    request.reply(web::http::status_codes::OK, U(json.dump()), U("application/x-download-me"));
}

void SessionSaveHandler::ImportCallback(web::http::http_request request) {
    request.extract_string().then([this,&request](const utility::string_t& body){
        try {
            nlohmann::json j = nlohmann::json::parse(body);
            j.get_to(session_);
            request.reply(web::http::status_codes::OK, U("Session import successful.")).wait();
        } catch(const std::exception& e) {
            std::string rep = U(std::string("Import session: ") + e.what());
            request.reply(web::http::status_codes::InternalError, rep).wait();
        }
    }).wait();
}
