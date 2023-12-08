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
                [](const auto& path) { return path == U("/session_backup.json"); },
                std::bind_front(&SessionSaveHandler::ExportCallback, this)
            },
            {
                web::http::methods::PUT,
                [](const auto& path) { return path == U("/session/import"); },
                std::bind_front(&SessionSaveHandler::ImportCallback, this)
            }
    };
}

void SessionSaveHandler::ExportCallback(const web::http::http_request &request) {
    const nlohmann::json json = session_;
    // Using MIME-type to force download instead of dumping JSON to the screen
    const auto json_dump = json.dump();
    request.reply(web::http::status_codes::OK, utility::string_t(json_dump.begin(),json_dump.end()), U("application/x-download-me"));
}

void SessionSaveHandler::ImportCallback(web::http::http_request request) {
    request.extract_string().then([this,&request](const utility::string_t& body){
        try {
            nlohmann::json j = nlohmann::json::parse(body);
            j.get_to(session_);
            request.reply(web::http::status_codes::OK, U("Session import successful.")).wait();
        } catch(const std::exception& e) {
            const std::string error_msg = e.what();
            utility::string_t rep = U("Import session: ") + utility::string_t(error_msg.begin(), error_msg.end());
            request.reply(web::http::status_codes::InternalError, rep).wait();
        }
    }).wait();
}
