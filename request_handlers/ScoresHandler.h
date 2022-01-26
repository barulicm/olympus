#ifndef OLYMPUS_SCORESHANDLER_H
#define OLYMPUS_SCORESHANDLER_H

#include "request_handler.h"
#include "common/Session.h"
#include "javascript_executor.h"

class ScoresHandler : public RequestHandler {
public:
    explicit ScoresHandler(Session& session, JavascriptExecutor& js);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    JavascriptExecutor& js_;
    void Callback(web::http::http_request request);
    void UpdateRanks();
    void UpdateResults();
    Team &GetTeamByNumber(const std::string &number);
};


#endif //OLYMPUS_SCORESHANDLER_H
