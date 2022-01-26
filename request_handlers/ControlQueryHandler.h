#ifndef OLYMPUS_CONTROLQUERYHANDLER_H
#define OLYMPUS_CONTROLQUERYHANDLER_H

#include "request_handler.h"
#include "common/Session.h"

class ControlQueryHandler : public RequestHandler {
public:
    explicit ControlQueryHandler(Session& session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    void Callback(const web::http::http_request& request);


};


#endif //OLYMPUS_CONTROLQUERYHANDLER_H
