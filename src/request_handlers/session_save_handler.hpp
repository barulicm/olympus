#ifndef OLYMPUS_SESSION_SAVE_HANDLER_H
#define OLYMPUS_SESSION_SAVE_HANDLER_H

#include "request_handler.hpp"
#include "session.hpp"

class SessionSaveHandler : public RequestHandler {
public:
    explicit SessionSaveHandler(Session& session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;

    void ExportCallback(const web::http::http_request& request);
    void ImportCallback(web::http::http_request request);


};


#endif //OLYMPUS_SESSION_SAVE_HANDLER_H
