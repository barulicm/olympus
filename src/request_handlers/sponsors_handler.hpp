#ifndef OLYMPUS_SPONSORS_HANDLER_HPP
#define OLYMPUS_SPONSORS_HANDLER_HPP

#include "request_handler.hpp"
#include "state_description/session.hpp"

class SponsorsHandler : public RequestHandler {
public:
    explicit SponsorsHandler(Session & session);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session & session_;

    bool PathPredicateGet(const utility::string_t& path);
    bool PathPredicatePut(const utility::string_t& path);
    void CallbackGet(const web::http::http_request & request);
    void CallbackPut(web::http::http_request request);

};

#endif //OLYMPUS_SPONSORS_HANDLER_HPP
