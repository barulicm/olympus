#ifndef OLYMPUS_SCHEDULEHANDLER_H
#define OLYMPUS_SCHEDULEHANDLER_H

#include "request_handler.h"
#include "common/Session.h"
#include "javascript_executor.h"

class ScheduleHandler : public RequestHandler {
public:
    ScheduleHandler(Session& session, JavascriptExecutor& javascript_executor);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    JavascriptExecutor& js_;
    void CallbackGet(const web::http::http_request& request);
    void CallbackPut(web::http::http_request request);
    void FillNextPhase();

};


#endif //OLYMPUS_SCHEDULEHANDLER_H
