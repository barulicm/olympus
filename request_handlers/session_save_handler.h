#ifndef OLYMPUS_SESSION_SAVE_HANDLER_H
#define OLYMPUS_SESSION_SAVE_HANDLER_H

#include "request_handler.h"
#include "common/session.h"

class SessionSaveHandler : public RequestHandler {
public:
    explicit SessionSaveHandler(Session& session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;

    void Callback(const web::http::http_request& request);


};


#endif //OLYMPUS_SESSION_SAVE_HANDLER_H
