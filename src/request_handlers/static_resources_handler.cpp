#include "static_resources_handler.hpp"
#include <cpprest/filestream.h>
#include "mime_types.hpp"

std::vector<RequestHandlerDetails> StaticResourceHandler::GetHandlers() {
    return {
            {web::http::methods::GET,
             std::bind_front(&StaticResourceHandler::PathPredicate, this),
             std::bind_front(&StaticResourceHandler::Callback, this)}
    };
}

bool StaticResourceHandler::PathPredicate(const utility::string_t &path) {
    return std::filesystem::exists(static_resources_dir / std::filesystem::path(path).relative_path()) || path == "/";
}

void StaticResourceHandler::Callback(const web::http::http_request &request) {
    std::filesystem::path path = request.relative_uri().path();
    if(path.empty() || path == "/") {
        path = "index.html";
    }
    if(path.is_absolute()) {
        path = path.relative_path();
    }
    path = static_resources_dir / path;
    concurrency::streams::fstream::open_istream(U(path.string()), std::ios::in)
            .then([this, &request, &path](const concurrency::streams::istream &is) {
                request.reply(web::http::status_codes::OK, is, GetMimeTypeForPath(path)).wait();
            }).wait();
}
