#include "config_handler.hpp"

ConfigHandler::ConfigHandler(Config &config)
    : config_(config)
{

}

std::vector<RequestHandlerDetails> ConfigHandler::GetHandlers() {
    return {
            {
                web::http::methods::GET,
                [](const auto& path) { return path == "/config"; },
                std::bind_front(&ConfigHandler::CallbackGet, this)
            },
            {
                web::http::methods::PUT,
                [](const auto& path) { return path == "/config"; },
                std::bind_front(&ConfigHandler::CallbackPut, this)
            }
    };
}

void ConfigHandler::CallbackGet(const web::http::http_request &request) {
    const auto name_header_iter = request.headers().find("name");
    if(name_header_iter == request.headers().end()) {
        request.reply(web::http::status_codes::BadRequest, U("Missing required header: name"), U("text/plain")).wait();
        return;
    }
    const auto name = name_header_iter->second;
    std::string response;
    if(name == "competition_name") {
        response = config_.competition_name;
    } else if(name == "show_timer") {
        response = (config_.show_timer ? "true" : "false");
    } else if(name == "rows_on_display") {
        response = std::to_string(config_.rows_on_display);
    } else {
        request.reply(web::http::status_codes::BadRequest, U("No such config value with name: '" + name + "'"), U("text/plain")).wait();
        return;
    }
    request.reply(web::http::status_codes::OK, response, U("text/plain")).wait();
}

void ConfigHandler::CallbackPut(const web::http::http_request &request) {
    const auto name_header_iter = request.headers().find("name");
    if(name_header_iter == request.headers().end()) {
        request.reply(web::http::status_codes::BadRequest, U("Missing required header: name"), U("text/plain")).wait();
        return;
    }
    const auto name = name_header_iter->second;
    const auto value_header_iter = request.headers().find("value");
    if(value_header_iter == request.headers().end()) {
        request.reply(web::http::status_codes::BadRequest, U("Missing required header: value"), U("text/plain")).wait();
        return;
    }
    const auto value = value_header_iter->second;
    if(name == "competition_name") {
        request.reply(web::http::status_codes::BadRequest, U("Config 'competition_name' is read-only."), U("text/plain")).wait();
    } else if(name == "show_timer") {
        config_.show_timer = value == "true";
        request.reply(web::http::status_codes::OK);
    } else if(name == "rows_on_display") {
        config_.rows_on_display = std::stoi(value);
        request.reply(web::http::status_codes::OK);
    } else {
        request.reply(web::http::status_codes::BadRequest, U("No such config value with name: '" + name + "'"), U("text/plain")).wait();
    }
}
