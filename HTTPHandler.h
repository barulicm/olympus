#ifndef OLYMPUS_HANDLER_H
#define OLYMPUS_HANDLER_H

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>
#include <cpprest/producerconsumerstream.h>
#include "common/Team.h"

class HTTPHandler {
public:
    HTTPHandler(){}
    HTTPHandler(utility::string_t url);
    virtual ~HTTPHandler(){}

    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

private:
    std::vector<Team> _teams;

    void handle_get(web::http::http_request message);
    void handle_put(web::http::http_request message);
    void handle_post(web::http::http_request message);
    void handle_delete(web::http::http_request message);
    web::http::experimental::listener::http_listener m_listener;
};


#endif //OLYMPUS_HANDLER_H
