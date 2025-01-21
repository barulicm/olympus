#ifndef OLYMPUS_TEAM_HANDLER_H
#define OLYMPUS_TEAM_HANDLER_H

#include "request_handler.hpp"
#include "state_description/session.hpp"

class TeamHandler : public RequestHandler {
public:
    explicit TeamHandler(Session &session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;

    void CallbackGet(const web::http::http_request& request);
    void CallbackPut(web::http::http_request request);
    Team& GetTeamByNumber(const std::string& number);
    bool TeamNumberExists(const std::string& number);

};


#endif //OLYMPUS_TEAM_HANDLER_H
