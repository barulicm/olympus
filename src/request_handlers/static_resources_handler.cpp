#include "static_resources_handler.hpp"
#include <cpprest/filestream.h>
#include "mime_types.hpp"

StaticResourceHandler::StaticResourceHandler(const std::filesystem::path &share_path)
: static_resources_dir(share_path / "static")
{
}

std::vector<RequestHandlerDetails> StaticResourceHandler::GetHandlers() {
    return {
            {web::http::methods::GET,
             std::bind_front(&StaticResourceHandler::PathPredicate, this),
             std::bind_front(&StaticResourceHandler::Callback, this)}
    };
}

bool StaticResourceHandler::PathPredicate(const utility::string_t &path) {

    return std::filesystem::exists(static_resources_dir / std::filesystem::path(path).relative_path().make_preferred()) || path == U("/");
}

void StaticResourceHandler::Callback(const web::http::http_request &request) {
    std::filesystem::path path = request.relative_uri().path();
    if(path.empty() || path == "/") {
        path = "index.html";
    }
    path = static_resources_dir / path.make_preferred().relative_path();
#ifdef _UTF16_STRINGS
    const auto path_str = path.wstring();
#else
    const auto path_str = path.string();
#endif
    concurrency::streams::fstream::open_istream(path_str, std::ios::in)
            .then([this, &request, &path](const concurrency::streams::istream &is) {

                request.reply(web::http::status_codes::OK, is, GetMimeTypeForPath(path)).wait();
            }).wait();
}
