#include "config_handler.hpp"

ConfigHandler::ConfigHandler(Config &config)
    : config_(config)
{

}

std::vector<RequestHandlerDetails> ConfigHandler::GetHandlers() {
    return {
            {
                web::http::methods::GET,
                [](const auto& path) { return path == U("/config"); },
                std::bind_front(&ConfigHandler::CallbackGet, this)
            },
            {
                web::http::methods::PUT,
                [](const auto& path) { return path == U("/config"); },
                std::bind_front(&ConfigHandler::CallbackPut, this)
            }
    };
}

void ConfigHandler::CallbackGet(const web::http::http_request &request) {
    const auto name_header_iter = request.headers().find(U("name"));
    if(name_header_iter == request.headers().end()) {
        request.reply(web::http::status_codes::BadRequest, U("Missing required header: name"), U("text/plain")).wait();
        return;
    }
    const auto name = name_header_iter->second;
    utility::string_t response;
    if(name == U("competition_name")) {
        response = utility::string_t(config_.competition_name.begin(), config_.competition_name.end());
    } else if(name == U("show_timer")) {
        response = (config_.show_timer ? U("true") : U("false"));
    } else if(name == U("rows_on_display")) {
#ifdef _UTF16_STRINGS
        response = std::to_wstring(config_.rows_on_display);
#else
        response = std::to_string(config_.rows_on_display);
#endif
    } else if(name == U("display_state")) {
        switch(config_.display_state) {
            case Config::DisplayState::ShowScores:
                response = U("ShowScores");
                break;
            case Config::DisplayState::Blackout:
                response = U("Blackout");
                break;
            case Config::DisplayState::FllLogo:
                response = U("FllLogo");
                break;
            case Config::DisplayState::Sponsors:
                response = U("Sponsors");
                break;
            default:
                response = U("unknown");
                break;
        }
    } else {
        request.reply(web::http::status_codes::BadRequest, U("No such config value with name: '") + name + U("'"), U("text/plain")).wait();
        return;
    }
    request.reply(web::http::status_codes::OK, response, U("text/plain")).wait();
}

void ConfigHandler::CallbackPut(const web::http::http_request &request) {
    const auto name_header_iter = request.headers().find(U("name"));
    if(name_header_iter == request.headers().end()) {
        request.reply(web::http::status_codes::BadRequest, U("Missing required header: name"), U("text/plain")).wait();
        return;
    }
    const auto name = name_header_iter->second;
    const auto value_header_iter = request.headers().find(U("value"));
    if(value_header_iter == request.headers().end()) {
        request.reply(web::http::status_codes::BadRequest, U("Missing required header: value"), U("text/plain")).wait();
        return;
    }
    const auto value = value_header_iter->second;
    if(name == U("competition_name")) {
        request.reply(web::http::status_codes::BadRequest, U("Config 'competition_name' is read-only."), U("text/plain")).wait();
    } else if(name == U("show_timer")) {
        config_.show_timer = value == U("true");
        request.reply(web::http::status_codes::OK);
    } else if(name == U("rows_on_display")) {
        config_.rows_on_display = std::stoi(value);
        request.reply(web::http::status_codes::OK);
    } else if(name == U("display_state")) {
        if(value == U("ShowScores")) {
            config_.display_state = Config::DisplayState::ShowScores;
            request.reply(web::http::status_codes::OK);
        } else if(value == U("Blackout")) {
            config_.display_state = Config::DisplayState::Blackout;
            request.reply(web::http::status_codes::OK);
        } else if(value == U("FllLogo")) {
            config_.display_state = Config::DisplayState::FllLogo;
            request.reply(web::http::status_codes::OK);
        } else if(value == U("Sponsors")) {
            config_.display_state = Config::DisplayState::Sponsors;
            request.reply(web::http::status_codes::OK);
        } else {
            request.reply(web::http::status_codes::BadRequest, U("Unrecognized value for DisplayState"));
        }
    } else {
        request.reply(web::http::status_codes::BadRequest, U("No such config value with name: '") + name + U("'"), U("text/plain")).wait();
    }
}
