#ifndef OLYMPUS_STATICRESOURCEHANDLER_H
#define OLYMPUS_STATICRESOURCEHANDLER_H

#include <filesystem>
#include "RequestHandler.h"

class StaticResourceHandler : public RequestHandler {
public:

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    const std::filesystem::path static_resources_dir = "resources/static/";
    bool PathPredicate(const utility::string_t& path);
    void Callback(const web::http::http_request& request);

};


#endif //OLYMPUS_STATICRESOURCEHANDLER_H
