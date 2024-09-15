#ifndef OLYMPUS_ANNOUNCEMENT_HANDLER_HPP
#define OLYMPUS_ANNOUNCEMENT_HANDLER_HPP

#include "request_handler.hpp"

class AnnouncementHandler : public RequestHandler {
public:

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    utility::string_t announcement_content_;
    bool announcement_visible_ = false;

    void CallbackGet(const web::http::http_request& request);
    void CallbackPut(web::http::http_request request);

};


#endif //OLYMPUS_ANNOUNCEMENT_HANDLER_HPP
