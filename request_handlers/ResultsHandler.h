#ifndef OLYMPUS_RESULTSHANDLER_H
#define OLYMPUS_RESULTSHANDLER_H

#include "RequestHandler.h"
#include "common/Session.h"

class ResultsHandler : public RequestHandler {
public:
    explicit ResultsHandler(Session& session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    void Callback(const web::http::http_request& request);

};


#endif //OLYMPUS_RESULTSHANDLER_H
