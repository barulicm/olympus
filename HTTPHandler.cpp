#include "HTTPHandler.h"

using namespace std;
using namespace web::http;

HTTPHandler::HTTPHandler(utility::string_t url, const utility::string_t &competitionName)
    : m_listener(url),
      competitionName(competitionName)
{
    m_listener.support(methods::GET, std::bind(&HTTPHandler::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::PUT, std::bind(&HTTPHandler::handle_put, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&HTTPHandler::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::DEL, std::bind(&HTTPHandler::handle_delete, this, std::placeholders::_1));

    {
        ifstream fileIn{"resources/dynamic/" + competitionName + "/scripts/CompareTeams.js"};
        string fileContents{istreambuf_iterator<char>{fileIn}, istreambuf_iterator<char>{}};
        _js.loadFunctionsFromString(fileContents);
    }
    {
        ifstream fileIn{"resources/dynamic/" + competitionName + "/scripts/GetTeamScore.js"};
        string fileContents{istreambuf_iterator<char>{fileIn}, istreambuf_iterator<char>{}};
        _js.loadFunctionsFromString(fileContents);
    }

    _mime_types = {
            {"html","text/html"},
            {"css","text/css"},
            {"js","text/javascript"},
            {"bmp","image/bmp"},
            {"gif","image/gif"},
            {"jpg","image/jpeg"},
            {"png","image/png"},
            {"txt","text/plain"}
    };
}

void HTTPHandler::handle_get(http_request message) {
    ucout << message.to_string() << endl;

    utility::string_t path = message.relative_uri().path();

    if(path == "/") {
        path = "/index.html";
    }

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
                message.reply(status_codes::NotFound, U("No such team")).wait();
            }
        }
    } else if (path.substr(0,10) == "/schedule/") {
        auto sched_request = path.substr(10);
        if(sched_request.substr(0,6) == "match/") {
            auto match_number = sched_request.substr(6);
            if(match_number == "current") {
                if(_schedule.isValid()) {
                    auto match = _schedule.phases[_schedule.currentPhase].matches[_schedule.currentMatch];
                    auto matchJson = match.toJSON();
                    matchJson["number"] = _schedule.currentMatch;
                    message.reply(status_codes::OK, matchJson.dump(), U("application/json")).wait();
                } else {
                    message.reply(status_codes::OK, "{\"ERROR\":\"No Matches Scheduled\"}", U("application/json")).wait();
                }
            }

        } else if(sched_request.substr(0,4) == "full") {
            auto schedule_json = _schedule.toJSON();
            message.reply(status_codes::OK, schedule_json.dump(), U("application/json")).wait();
        } else {
            message.reply(status_codes::NotFound, U("Unrecognized schedule request")).wait();
        }

    } else if(file_exists("resources/dynamic/" + competitionName + path)) {
        // Request for competition-specific resource

        concurrency::streams::fstream::open_istream(U("resources/dynamic/" + competitionName + path), std::ios::in)
                .then([=](concurrency::streams::istream is) {
                    message.reply(status_codes::OK, is, mime_type_for_path(path)).wait();
                }).wait();

    } else if(file_exists("resources/static" + path)) {
        // Request for static resource

        concurrency::streams::fstream::open_istream(U("resources/static" + path), std::ios::in)
                .then([=](concurrency::streams::istream is) {
                    message.reply(status_codes::OK, is, mime_type_for_path(path)).wait();
                }).wait();

    } else {
        message.reply(status_codes::NotFound, U("PAGE NOT FOUND")).wait();
    }
}

void HTTPHandler::handle_put(http_request message) {
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
                message.reply(status_codes::OK, rep).wait();
            } catch(...) {
                string rep = U("Add team failed.");
                message.reply(status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(message.relative_uri().path() == "/team/remove") {
        message.extract_string().then([this,&message](utility::string_t body){
            try {
                json j = json::parse(body);
                std::string teamNumber = j["number"];
                auto findIter = std::find_if(_teams.begin(), _teams.end(), [&teamNumber](const auto &team){
                    return team.number == teamNumber;
                });
                if(findIter == _teams.end()) {
                    string rep = U("No such team.");
                    message.reply(status_codes::NotFound, rep).wait();
                } else {
                    _teams.erase(findIter);
                    string rep = U("Remove team successful.");
                    message.reply(status_codes::OK, rep).wait();
                }
            } catch(...) {
                string rep = U("Remove team failed.");
                message.reply(status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(message.relative_uri().path() == "/scores/submit") {
        message.extract_string().then([this,&message](utility::string_t body){
            try {
                json j = json::parse(body);

                string teamNumber = j["teamNumber"];
                int score = j["score"];

                auto team_iter = find_if(_teams.begin(),_teams.end(),
                                         [&teamNumber](const Team &t) {
                                             return t.number == teamNumber;
                                         });
                if(team_iter != _teams.end()) {
                    std::cout << "Found team: " << team_iter->name << std::endl;
                    team_iter->scores[_schedule.currentPhase].push_back(score);
                    stable_sort(_teams.begin(), _teams.end(),
                                [=](const Team &a, const Team &b){
                                    json response = _js.callFunction("CompareTeams",{a.toJSON(),b.toJSON()});
                                    return response["result"];
                                });
                    std::cout << "Teams sorted." << std::endl;
                    for_each(_teams.begin(), _teams.end(),
                             [=](Team &team){
                                 std::cout << "Getting score for team " << team.name << std::endl;
                                 json response = _js.callFunction("GetTeamScore",{team.toJSON()});
                                 std::cout << "Is Null: " << response.is_null() << std::endl;
                                 std::cout << "Is Discarded: " << response.is_discarded() << std::endl;
                                 std::cout << "\tStoring score." << std::endl;
                                 std::cout << response << std::endl;
                                 team.displayScore = response["score"];
                                 std::cout << "\tScore update done." << std::endl;
                             });
                    std::cout << "Team display scores updated." << std::endl;
                    int rank = 1;
                    _teams[0].rank=rank;
                    for(size_t i = 1; i < _teams.size(); i++) {
                        if(_js.callFunction("CompareTeams",{_teams[i-1].toJSON(),_teams[i].toJSON()})["result"]) {
                            rank++;
                        }
                        _teams[i].rank = rank;
                    }
                    std::cout << "Team ranks updated." << std::endl;
                } else {
                    string rep = U("Score submission failed. Nonexistent team number.");
                    message.reply(status_codes::NotFound, rep).wait();
                    return;
                }

                string rep = U("Score submission successful.");
                message.reply(status_codes::OK, rep).wait();
            } catch(const std::exception &e) {
                string rep = U("Score submission failed.");
                message.reply(status_codes::InternalError, rep).wait();
                std::cerr << e.what() << std::endl;
            }
        }).wait();
    } else if(message.relative_uri().path() == "/schedule/load") {
        message.extract_string().then([this,&message](utility::string_t body){
            try {
                _schedule.phases.clear();

                json j = json::parse(body);

                auto phases = j["phases"];

                for(const auto &phase : phases) {
                    _schedule.phases.emplace_back();
                    _schedule.phases.back().name = phase["name"];
                    auto matches = phase["matches"];
                    for(const auto &match : matches) {
                        _schedule.phases.back().matches.emplace_back();
                        auto teams = match["teams"];
                        for(const auto &team : teams) {
                            _schedule.phases.back().matches.back().team_numbers.push_back(team);
                        }
                    }
                }
                _schedule.currentPhase = 0;
                _schedule.currentMatch = 0;

                for(auto &team : _teams) {
                    team.scores.resize(_schedule.phases.size());
                }

                string rep = U("Schedule loading successful.");
                message.reply(status_codes::OK, rep).wait();

                cout << _schedule.getCurrentPhase().matches.size() << endl;

            } catch(const std::exception &e) {
                string rep = U("Schedule loading failed. ");
                rep += e.what();
                message.reply(status_codes::InternalError, rep).wait();
                std::cerr << e.what() << std::endl;
            }
        }).wait();
    }
}

void HTTPHandler::handle_post(http_request message) {
    ucout <<  message.to_string() << endl;

    message.reply(status_codes::OK,message.to_string()).wait();
}

void HTTPHandler::handle_delete(http_request message) {
    ucout <<  message.to_string() << endl;

    string rep = U("WRITE YOUR OWN DELETE OPERATION");
    message.reply(status_codes::OK,rep).wait();
}

bool HTTPHandler::file_exists(string filename) {
    ifstream f{filename};
    return f.good();
}

std::string HTTPHandler::mime_type_for_path(std::string path) {
    auto extension = path.substr(path.find_last_of('.') + 1);
    auto mime_type = _mime_types[extension];
    if(mime_type.empty())
        mime_type = "text/plain";
    return mime_type;
}
