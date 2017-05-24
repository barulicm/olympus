#include "HTTPHandler.h"

using namespace std;
using namespace web::http;

HTTPHandler::HTTPHandler(utility::string_t url)
    : m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&HTTPHandler::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::PUT, std::bind(&HTTPHandler::handle_put, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&HTTPHandler::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::DEL, std::bind(&HTTPHandler::handle_delete, this, std::placeholders::_1));

    _teams = {{1,"0000","name1",{}},{2,"0001","name2",{}}};
}

void HTTPHandler::handle_get(http_request message) {
    ucout << message.to_string() << endl;

    utility::string_t path = message.relative_uri().path();

    if(path.substr(0,6) == "/team/") {
        // Request for individual team data
        auto team_number = path.substr(6);
        if(team_number == "all") {
            json j = json::array();
            for(const auto &team : _teams) {
                j.push_back(team.toJSON());
            }
            message.reply(status_codes::OK, j.dump(), U("application/json")).wait();
        } else {
            auto team_iter = std::find_if(_teams.begin(), _teams.end(),
                                          [&team_number](const Team &t){
                                              return t.number == team_number;
                                          });
            if(team_iter != _teams.end()) {
                message.reply(status_codes::OK, team_iter->toJSON().dump(), U("application/json")).wait();
            } else {
                message.reply(status_codes::InternalError, U("INTERNAL ERROR"));
            }
        }
    } else {
        // Request for static resource
        if(path == "/") {
            path = "/index.html";
        }

        utility::string_t mimeType = U("text/html");
        if(path.find(".css") != utility::string_t::npos) {
            mimeType = U("text/css");
        }

        concurrency::streams::fstream::open_istream(U("resources/static" + path), std::ios::in)
                .then([=](concurrency::streams::istream is) {
                    message.reply(status_codes::OK, is, mimeType).wait();
                })
                .then([=](pplx::task<void> t) {
                    try {
                        t.get();
                    } catch (...) {
                        message.reply(status_codes::InternalError, U("INTERNAL ERROR"));
                    }
                });
    }
}

void HTTPHandler::handle_put(http_request message) {
    ucout <<  message.to_string() << endl;

    if(message.relative_uri().path() == "/team/add") {
        message.extract_string().then([this,&message](utility::string_t body){
            try {
                json j = json::parse(body);
                Team newTeam;
                newTeam.rank = 0;
                newTeam.number = j["number"];
                newTeam.name = j["name"];
                _teams.push_back(newTeam);
                string rep = U("Add team successful.");
                message.reply(status_codes::OK, rep);
            } catch(...) {
                string rep = U("Add team failed.");
                message.reply(status_codes::InternalError, rep);
            }
        }).wait();
    }
}

void HTTPHandler::handle_post(http_request message) {
    ucout <<  message.to_string() << endl;

    message.reply(status_codes::OK,message.to_string());
    return ;
}

void HTTPHandler::handle_delete(http_request message) {
    ucout <<  message.to_string() << endl;

    string rep = U("WRITE YOUR OWN DELETE OPERATION");
    message.reply(status_codes::OK,rep);
    return;
}
