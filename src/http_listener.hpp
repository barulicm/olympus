#ifndef OLYMPUS_HANDLER_HPP
#define OLYMPUS_HANDLER_HPP

#include <thread>
#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>
#include <cpprest/producerconsumerstream.h>
#include "request_handler_details.hpp"

class HttpListener {
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


#endif //OLYMPUS_HANDLER_HPP
