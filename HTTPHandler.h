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
#include "JSExecutor.h"
#include "common/Schedule.h"

class HTTPHandler {
public:
    HTTPHandler() = default;
    HTTPHandler(utility::string_t url, const utility::string_t &competitionName);
    virtual ~HTTPHandler() = default;

    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

private:
    std::vector<Team> _teams;

    Schedule _schedule;

    JSExecutor _js;

    void handle_get(web::http::http_request message);
    void handle_put(web::http::http_request message);
    void handle_post(web::http::http_request message);
    void handle_delete(web::http::http_request message);
    web::http::experimental::listener::http_listener m_listener;

    bool file_exists(std::string filename);

    std::string mime_type_for_path(std::string path);

    std::map<std::string,std::string> _mime_types;

    utility::string_t competitionName;
};


#endif //OLYMPUS_HANDLER_H
