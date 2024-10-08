#ifndef OLYMPUS_CONTROL_QUERY_HANDLER_H
#define OLYMPUS_CONTROL_QUERY_HANDLER_H

#include "request_handler.hpp"
#include "state_description/session.hpp"

class ControlQueryHandler : public RequestHandler {
public:
    explicit ControlQueryHandler(Session& session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    void Callback(const web::http::http_request& request);


};


#endif //OLYMPUS_CONTROL_QUERY_HANDLER_H
