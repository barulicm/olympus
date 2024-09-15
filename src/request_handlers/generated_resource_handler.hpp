#ifndef OLYMPUS_GENERATED_RESOURCE_HANDLER_HPP
#define OLYMPUS_GENERATED_RESOURCE_HANDLER_HPP

#include "state_description/session.hpp"
#include "request_handler.hpp"

class GeneratedResourceHandler : public RequestHandler {
public:
    explicit GeneratedResourceHandler(Session & session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    bool PathPredicate(const utility::string_t& path);
    void Callback(const web::http::http_request& request);


};


#endif //OLYMPUS_GENERATED_RESOURCE_HANDLER_HPP
