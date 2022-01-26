#ifndef OLYMPUS_SCHEDULEHANDLER_H
#define OLYMPUS_SCHEDULEHANDLER_H

#include "RequestHandler.h"
#include "common/Session.h"
#include "JSExecutor.h"

class ScheduleHandler : public RequestHandler {
public:
    ScheduleHandler(Session& session, JSExecutor& javascript_executor);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session& session_;
    JSExecutor& js_;
    void CallbackGet(const web::http::http_request& request);
    void CallbackPut(web::http::http_request request);
    void FillNextPhase();

};


#endif //OLYMPUS_SCHEDULEHANDLER_H
