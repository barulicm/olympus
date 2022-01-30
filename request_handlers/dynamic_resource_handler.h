#ifndef OLYMPUS_DYNAMIC_RESOURCE_HANDLER_H
#define OLYMPUS_DYNAMIC_RESOURCE_HANDLER_H

#include <filesystem>
#include "request_handler.h"

class DynamicResourceHandler : public RequestHandler {
public:
    DynamicResourceHandler(const std::string& competition_name);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    std::filesystem::path dynamic_resources_dir;
    bool PathPredicate(const utility::string_t& path);
    void Callback(const web::http::http_request& request);

};

#endif //OLYMPUS_DYNAMIC_RESOURCE_HANDLER_H
