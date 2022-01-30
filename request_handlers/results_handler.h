#ifndef OLYMPUS_RESULTS_HANDLER_H
#define OLYMPUS_RESULTS_HANDLER_H

#include "request_handler.h"
#include "common/session.h"

class ResultsHandler : public RequestHandler {
public:
    explicit ResultsHandler(Session& session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    void Callback(const web::http::http_request& request);

};


#endif //OLYMPUS_RESULTS_HANDLER_H
