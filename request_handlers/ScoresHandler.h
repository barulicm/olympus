#ifndef OLYMPUS_SCORESHANDLER_H
#define OLYMPUS_SCORESHANDLER_H

#include "RequestHandler.h"
#include "common/Session.h"
#include "JSExecutor.h"

class ScoresHandler : public RequestHandler {
public:
    explicit ScoresHandler(Session& session, JSExecutor& js);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    JSExecutor& js_;
    void Callback(web::http::http_request request);
    void UpdateRanks();
    void UpdateResults();
    Team &GetTeamByNumber(const std::string &number);
};


#endif //OLYMPUS_SCORESHANDLER_H
