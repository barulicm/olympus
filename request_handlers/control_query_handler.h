#ifndef OLYMPUS_CONTROL_QUERY_HANDLER_H
#define OLYMPUS_CONTROL_QUERY_HANDLER_H

#include "request_handler.h"
#include "common/session.h"

class ControlQueryHandler : public RequestHandler {
public:
    explicit ControlQueryHandler(Session& session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    void Callback(const web::http::http_request& request);


};


#endif //OLYMPUS_CONTROL_QUERY_HANDLER_H
