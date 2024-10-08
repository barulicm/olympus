#include "http_listener.hpp"

HttpListener::HttpListener(web::uri uri)
    : m_listener(std::move(uri))
{
    m_listener.support(web::http::methods::GET, std::bind_front(&HttpListener::handle_request, this));
    m_listener.support(web::http::methods::PUT, std::bind_front(&HttpListener::handle_request, this));
    m_listener.support(web::http::methods::POST, std::bind_front(&HttpListener::handle_request, this));
    m_listener.support(web::http::methods::DEL, std::bind_front(&HttpListener::handle_request, this));
}

void HttpListener::registerRequestHandler(RequestHandlerDetails details) {
    _requestHandlers.push_back(std::move(details));
}

void HttpListener::handle_request(const web::http::http_request &request) {
    auto found_handler_iter = std::find_if(_requestHandlers.begin(), _requestHandlers.end(), [&request](const auto& handler){
        return handler.method == request.method() && handler.pathPredicate(request.relative_uri().path());
    });
    if(found_handler_iter == _requestHandlers.end()) {
        request.reply(web::http::status_codes::NotFound, U("Resource not found")).wait();
        return;
    }
    found_handler_iter->callback(request);
}
