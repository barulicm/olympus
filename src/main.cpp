#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind/bind.hpp>
#include "http_listener.hpp"
#include "request_handlers/all_request_handlers.hpp"
#include "session.hpp"
#include "get_exec_path.hpp"
#include "get_ip_addresses.hpp"
#include "save_session_backup.hpp"

int main(int argc, char** argv) {
    Session session;

    try {
        const auto share_path = GetCurrentExecutableDirectory().parent_path().parent_path() / std::filesystem::path("share/olympus").make_preferred();
#ifdef __unix__
        const auto url = U("http://0.0.0.0:8080"); // listens on all interfaces on Linux
#elif _WIN32
        const auto url = U("http://127.0.0.1:8080"); // TODO make this work on all interfaces on windows
#else
#warning "Unsupported OS."
        const auto url = U("http://127.0.0.1:8080");
#endif
        web::uri_builder uri{url};
        const auto address = uri.to_uri().to_string();

        HttpListener http_listener;
        http_listener.setUrl(address);

        std::vector<std::unique_ptr<RequestHandler>> request_handlers;
        request_handlers.push_back(std::make_unique<SessionSaveHandler>(session));
        request_handlers.push_back(std::make_unique<TeamHandler>(session));
        request_handlers.push_back(std::make_unique<TimerHandler>());
        request_handlers.push_back(std::make_unique<ScoresHandler>(session));
        request_handlers.push_back(std::make_unique<ConfigHandler>(session.config));
        request_handlers.push_back(std::make_unique<ControlQueryHandler>(session));
        request_handlers.push_back(std::make_unique<StaticResourceHandler>(share_path));

        for (auto &handler: request_handlers) {
            for (const auto &details: handler->GetHandlers()) {
                http_listener.registerRequestHandler(details);
            }
        }

        http_listener.open().wait();

        const auto ip_addresses = GetIpAddresses();

        std::cout << "Listening for requests at:\n";

        for (const auto &ip_address: ip_addresses) {
            std::cout << "\thttp://" << ip_address << ":8080\n";
        }
        std::cout << std::endl;

        boost::asio::io_service signal_handling_io_service;
        boost::asio::signal_set signals(signal_handling_io_service);
        signals.add(SIGINT);
        signals.add(SIGTERM);
        signals.async_wait(boost::bind(&boost::asio::io_service::stop, &signal_handling_io_service));
        signal_handling_io_service.run();

        std::cout << "Server closed." << std::endl;

        http_listener.close().wait();
    }catch(const std::exception& e) {
        std::cerr << "Uncaught exception!\n" << e.what() << std::endl;
    }

    SaveSessionBackup(session);

    return 0;
}
