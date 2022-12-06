#ifndef OLYMPUS_REQUEST_HANDLER_DETAILS_H
#define OLYMPUS_REQUEST_HANDLER_DETAILS_H

#include <functional>
#include <cpprest/http_msg.h>

struct RequestHandlerDetails {
    web::http::method method;
    std::function<bool(const utility::string_t&)> pathPredicate;
    std::function<void(const web::http::http_request&)> callback;
};

#endif //OLYMPUS_REQUEST_HANDLER_DETAILS_H
