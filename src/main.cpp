#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind/bind.hpp>
#include "http_listener.hpp"
#include "request_handlers/all_request_handlers.hpp"
#include "state_description/session.hpp"
#include "get_exec_path.hpp"
#include "get_ip_addresses.hpp"
#include "save_session_backup.hpp"
#include "version.hpp"

int main(int argc, char** argv) {
    Session session;

    std::cout << "Olympus " << OLYMPUS_VERSION << std::endl;

    try {
        const auto share_path = GetCurrentExecutableDirectory().parent_path().parent_path() / std::filesystem::path("share/olympus").make_preferred();

        web::uri_builder uri_builder;
        uri_builder.set_scheme(U("http"));
        uri_builder.set_port(8080);

#ifdef __unix__
        uri_builder.set_host(U("0.0.0.0")); // listens on all interfaces on Linux
#elif _WIN32
        uri_builder.set_host(U("+"));
#else
#warning "Unsupported OS."
        uri_builder.set_host(U("127.0.0.1"));
#endif

        HttpListener http_listener(uri_builder.to_uri());

        std::vector<std::unique_ptr<RequestHandler>> request_handlers;
        request_handlers.push_back(std::make_unique<SessionSaveHandler>(session));
        request_handlers.push_back(std::make_unique<TeamHandler>(session));
        request_handlers.push_back(std::make_unique<TimerHandler>());
        request_handlers.push_back(std::make_unique<ScoresHandler>(session));
        request_handlers.push_back(std::make_unique<ConfigHandler>(session.config));
        request_handlers.push_back(std::make_unique<ControlQueryHandler>(session));
        request_handlers.push_back(std::make_unique<GameDefinitionHandler>(session, share_path / "game_configs"));
        request_handlers.push_back(std::make_unique<GeneratedResourceHandler>(session));
        request_handlers.push_back(std::make_unique<AnnouncementHandler>());
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
        std::cout << "\n";
        std::cout << "Press Ctrl+C to quit.\n";
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
