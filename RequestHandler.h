#ifndef OLYMPUS_REQUESTHANDLER_H
#define OLYMPUS_REQUESTHANDLER_H

#include <vector>
#include "RequestHandlerDetails.h"

struct RequestHandler {
    virtual ~RequestHandler() = default;
    virtual std::vector<RequestHandlerDetails> GetHandlers() = 0;
};

#endif //OLYMPUS_REQUESTHANDLER_H
