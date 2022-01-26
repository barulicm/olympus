#include <cpprest/filestream.h>
#include "DynamicResourceHandler.h"
#include "MimeTypes.h"
#include "json.hpp"

DynamicResourceHandler::DynamicResourceHandler(const std::string &competition_name)
        : dynamic_resources_dir("resources/dynamic/" + competition_name) {

}

std::vector<RequestHandlerDetails> DynamicResourceHandler::GetHandlers() {
    return {{
                    web::http::methods::GET,
                    std::bind_front(&DynamicResourceHandler::PathPredicate, this),
                    std::bind_front(&DynamicResourceHandler::Callback, this)
            }};
}

bool DynamicResourceHandler::PathPredicate(const utility::string_t &path) {
    if(path.empty() || path == "/") {
        return false;
    }
    return path == "/pages/dynamic" || std::filesystem::exists(dynamic_resources_dir / std::filesystem::path(path).relative_path());
}

void DynamicResourceHandler::Callback(const web::http::http_request &request) {
    if (request.relative_uri().path() == "/pages/dynamic") {
        auto j = nlohmann::json::array();
        for (const auto &dir_entry: std::filesystem::recursive_directory_iterator(dynamic_resources_dir)) {
            if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".html") {
                j.push_back(dir_entry.path().stem().string());
            }
        }
        request.reply(web::http::status_codes::OK, j.dump(), U("application/json")).wait();
    } else {
        const auto path = dynamic_resources_dir / std::filesystem::path(request.relative_uri().path()).relative_path();
        concurrency::streams::fstream::open_istream(U(path.string()), std::ios::in)
                .then([this, &request, &path](const concurrency::streams::istream &is) {
                    request.reply(web::http::status_codes::OK, is, GetMimeTypeForPath(path)).wait();
                }).wait();
    }
}
