#ifndef OLYMPUS_CONFIG_HANDLER_H
#define OLYMPUS_CONFIG_HANDLER_H

#include "request_handler.hpp"
#include "state_description/config.hpp"

class ConfigHandler : public RequestHandler {
public:
    explicit ConfigHandler(Config& config);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Config& config_;
    void CallbackGet(const web::http::http_request& request);
    void CallbackPut(const web::http::http_request& request);

};


#endif //OLYMPUS_CONFIG_HANDLER_H
