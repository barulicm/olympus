#ifndef OLYMPUS_HANDLER_H
#define OLYMPUS_HANDLER_H

#include <thread>
#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>
#include <cpprest/producerconsumerstream.h>
#include "JSExecutor.h"
#include "RequestHandlerDetails.h"
#include "common/Session.h"

class HTTPHandler {
public:
    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

    void setUrl(const utility::string_t &url);

    void registerRequestHandler(RequestHandlerDetails details);

private:
    std::vector<RequestHandlerDetails> _requestHandlers;
    web::http::experimental::listener::http_listener m_listener;
    void handle_request(const web::http::http_request &request);
};


#endif //OLYMPUS_HANDLER_H
