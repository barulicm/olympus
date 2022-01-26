#ifndef OLYMPUS_TEAMHANDLER_H
#define OLYMPUS_TEAMHANDLER_H

#include "request_handler.h"
#include "common/Session.h"

class TeamHandler : public RequestHandler {
public:
    explicit TeamHandler(Session &session, nlohmann::json default_custom_fields);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    nlohmann::json _defaultCustomFields;

    void CallbackGet(const web::http::http_request& request);
    void CallbackPut(web::http::http_request request);
    Team& GetTeamByNumber(const std::string& number);

};


#endif //OLYMPUS_TEAMHANDLER_H
