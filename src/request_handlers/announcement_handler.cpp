#include "announcement_handler.hpp"
#include <nlohmann/json.hpp>
#include "utilities/string_utilities.hpp"

std::vector<RequestHandlerDetails> AnnouncementHandler::GetHandlers() {
    auto path_predicate = [](const auto &path) {
        return path == U("/announcement");
    };
    return {
            {web::http::methods::GET,
                    path_predicate,
                    std::bind_front(&AnnouncementHandler::CallbackGet, this)},
            {web::http::methods::PUT,
                    path_predicate,
                    std::bind_front(&AnnouncementHandler::CallbackPut, this)}
    };
}

void AnnouncementHandler::CallbackGet(const web::http::http_request &request) {
    nlohmann::json data;
    data["visible"] = announcement_visible_;
    data["content"] = std::string(announcement_content_.begin(), announcement_content_.end());
    request.reply(web::http::status_codes::OK, olympus::utilities::ToUString(data.dump()), U("test/json"));
}

void AnnouncementHandler::CallbackPut(web::http::http_request request) {
    request.extract_string().then([this, &request](const utility::string_t &body) {
        try {
            nlohmann::json j = nlohmann::json::parse(body);
            // splitting getting and setting into separate steps to make updating the announcement atomic
            const auto visible = j["visible"].get<bool>();
            const auto content = j["content"].get<std::string>();
            announcement_visible_ = visible;
            announcement_content_ = olympus::utilities::ToUString(content);
            request.reply(web::http::status_codes::OK, U("Announcement set successfully.")).wait();
        } catch (const std::exception &e) {
            auto reply = U("Setting announcement failed: ") + olympus::utilities::ToUString(e.what());
            request.reply(web::http::status_codes::InternalError, reply).wait();
        }
    }).wait();
}
