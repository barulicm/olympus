#include "HTTPHandler.h"

void HTTPHandler::setUrl(const utility::string_t &url) {
    m_listener = web::http::experimental::listener::http_listener{url};
    m_listener.support(web::http::methods::GET, std::bind_front(&HTTPHandler::handle_request, this));
    m_listener.support(web::http::methods::PUT, std::bind_front(&HTTPHandler::handle_request, this));
    m_listener.support(web::http::methods::POST, std::bind_front(&HTTPHandler::handle_request, this));
    m_listener.support(web::http::methods::DEL, std::bind_front(&HTTPHandler::handle_request, this));
}

void HTTPHandler::registerRequestHandler(RequestHandlerDetails details) {
    _requestHandlers.push_back(std::move(details));
}

void HTTPHandler::handle_request(const web::http::http_request &request) {
    auto found_handler_iter = std::find_if(_requestHandlers.begin(), _requestHandlers.end(), [&request](const auto& handler){
        return handler.method == request.method() && handler.pathPredicate(request.relative_uri().path());
    });
    if(found_handler_iter == _requestHandlers.end()) {
        request.reply(web::http::status_codes::NotFound, U("Resource not found")).wait();
        return;
    }
    found_handler_iter->callback(request);
}
