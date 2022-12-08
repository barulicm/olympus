#include "http_listener.hpp"
#include "request_handlers/all_request_handlers.hpp"
#include "session.hpp"
#include "get_exec_path.hpp"
#include "get_ip_addresses.hpp"

int main(int argc, char** argv) {
    const auto share_path = GetCurrentExecutableDirectory() / "../share/olympus";

    const auto url = U("http://0.0.0.0:8080"); // listens on all interfaces on Linux
    web::uri_builder uri{url};
    const auto address = uri.to_uri().to_string();

    Session session;

    HttpListener http_listener;
    http_listener.setUrl(address);

    std::vector<std::unique_ptr<RequestHandler>> request_handlers;
    request_handlers.push_back(std::make_unique<SessionSaveHandler>(session));
    request_handlers.push_back(std::make_unique<TeamHandler>(session));
    request_handlers.push_back(std::make_unique<TimerHandler>());
    request_handlers.push_back(std::make_unique<ScoresHandler>(session));
    request_handlers.push_back(std::make_unique<ResultsHandler>(session));
    request_handlers.push_back(std::make_unique<ConfigHandler>(session.config));
    request_handlers.push_back(std::make_unique<ControlQueryHandler>(session));
    request_handlers.push_back(std::make_unique<StaticResourceHandler>(share_path));

    for(auto& handler : request_handlers) {
        for(const auto& details : handler->GetHandlers()) {
            http_listener.registerRequestHandler(details);
        }
    }

    http_listener.open().wait();

    const auto ip_addresses = GetIpAddresses();

    std::cout << "Listening for requests at:\n";

    for(const auto& ip_address : ip_addresses) {
        std::cout << "\thttp://" << ip_address << ":8080\n";
    }

    std::cout << "Press ENTER to exit.\n";

    std::cin.ignore();
    std::string line;
    std::getline(std::cin, line);

    http_listener.close().wait();

    return 0;
}
