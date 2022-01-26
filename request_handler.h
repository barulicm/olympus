#ifndef OLYMPUS_REQUEST_HANDLER_H
#define OLYMPUS_REQUEST_HANDLER_H

#include <vector>
#include "request_handler_details.h"

struct RequestHandler {
    virtual ~RequestHandler() = default;
    virtual std::vector<RequestHandlerDetails> GetHandlers() = 0;
};

#endif //OLYMPUS_REQUEST_HANDLER_H
