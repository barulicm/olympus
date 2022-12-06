#ifndef OLYMPUS_SCORES_HANDLER_H
#define OLYMPUS_SCORES_HANDLER_H

#include "request_handler.hpp"
#include "session.hpp"

class ScoresHandler : public RequestHandler {
public:
    explicit ScoresHandler(Session& session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    void CallbackPut(web::http::http_request request);
    void CallbackGet(web::http::http_request request);
    void UpdateRanks();
    void UpdateResults();
    Team &GetTeamByNumber(const std::string &number);
};


#endif //OLYMPUS_SCORES_HANDLER_H
