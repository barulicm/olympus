#include "sponsors_handler.hpp"
#include <nlohmann/json.hpp>

SponsorsHandler::SponsorsHandler(Session & session)
 : session_(session)
{
}

std::vector<RequestHandlerDetails> SponsorsHandler::GetHandlers() {
    return {{
                    web::http::methods::GET,
                    std::bind_front(&SponsorsHandler::PathPredicateGet, this),
                    std::bind_front(&SponsorsHandler::CallbackGet, this)
            },
            {
                    web::http::methods::PUT,
                    std::bind_front(&SponsorsHandler::PathPredicatePut, this),
                    std::bind_front(&SponsorsHandler::CallbackPut, this)
            }};
}

bool SponsorsHandler::PathPredicateGet(const utility::string_t &path) {
    return path == U("/sponsors");
}

bool SponsorsHandler::PathPredicatePut(const utility::string_t &path) {
    return path == U("/sponsors/add") || path == U("/sponsors/delete") || path == U("/sponsors/delete_all");
}

void SponsorsHandler::CallbackGet(const web::http::http_request &request) {
    nlohmann::json reply_json;
    reply_json["sponsors"] = session_.sponsors;
    const auto json_dump = reply_json.dump();
    request.reply(web::http::status_codes::OK, utility::string_t(json_dump.begin(), json_dump.end())).wait();
}

void SponsorsHandler::CallbackPut(web::http::http_request request) {
    const auto path = request.relative_uri().path();
    if (path == U("/sponsors/add")) {
        try {
            const auto body = request.extract_string().get();
            const auto body_json = nlohmann::json::parse(body);
            const auto image_data = body_json.at("image_data").get<std::string>();
            if(!image_data.starts_with("data:image/")) {
                request.reply(web::http::status_codes::BadRequest, U("Failed to add sponsor: File not recognized as an image.")).wait();
            }
            session_.sponsors.push_back(image_data);
            request.reply(web::http::status_codes::OK).wait();
        } catch (const std::exception &e) {
            const std::string error_msg = e.what();
            utility::string_t rep = U("Failed to add sponsor: ") + utility::string_t(error_msg.begin(), error_msg.end());
            request.reply(web::http::status_codes::InternalError, rep).wait();
        }
    } else if (path == U("/sponsors/delete")) {
        try {
            const auto body = request.extract_string().get();
            const auto body_json = nlohmann::json::parse(body);
            const auto index = body_json.at("index").get<int>();
            if(index < 0 || index > session_.sponsors.size()) {
                request.reply(web::http::status_codes::BadRequest, U("Failed to add sponsor: Index out of range.")).wait();
            }
            session_.sponsors.erase(session_.sponsors.begin() + index);
            request.reply(web::http::status_codes::OK).wait();
        } catch (const std::exception &e) {
            const std::string error_msg = e.what();
            utility::string_t rep = U("Failed to add sponsor: ") + utility::string_t(error_msg.begin(), error_msg.end());
            request.reply(web::http::status_codes::InternalError, rep).wait();
        }
    } else if (path == U("/sponsors/delete_all")) {
        session_.sponsors.clear();
        request.reply(web::http::status_codes::OK).wait();
    }
}
