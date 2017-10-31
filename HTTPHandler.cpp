#include <set>
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

    loadFunctionsFromJS("CompareTeams.js");
    loadFunctionsFromJS("GetTeamScore.js");
    loadFunctionsFromJS("SelectNextPhase.js");

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
//    ucout << message.to_string() << endl;

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
        } else if (team_number == "active") {
            json j = json::array();
            auto activeTeamNumbers = _schedule.getCurrentPhase().getInvolvedTeamNumbers();
            std::vector<Team> activeTeams = getTeamsFromNumbers(activeTeamNumbers.begin(), activeTeamNumbers.end());
            for(const auto &team : activeTeams) {
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
                    message.reply(status_codes::OK, R"rawdelim({"ERROR":"No Matches Scheduled"})rawdelim", U("application/json")).wait();
                }
            }

        } else if(sched_request.substr(0,4) == "full") {
            auto schedule_json = _schedule.toJSON();
            message.reply(status_codes::OK, schedule_json.dump(), U("application/json")).wait();
        } else {
            message.reply(status_codes::NotFound, U("Unrecognized schedule request")).wait();
        }

    } else if (path.substr(0,13) == "/controlQuery") {
        auto query = message.headers()["query"];
        std::string response;
        if(query == "hasTeams") {
            response = ( !_teams.empty() ? "true" : "false");
        } else if(query == "hasSchedule") {
            response = ( !_schedule.phases.empty() ? "true" : "false");
        }
        message.reply(status_codes::OK, response, U("text/plain")).wait();
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

                    updateRanks();

                    updateResults();

                    string rep = U("Score submission successful.");
                    message.reply(status_codes::OK, rep).wait();
                } else {
                    string rep = U("Score submission failed. Nonexistent team number.");
                    message.reply(status_codes::NotFound, rep).wait();
                    return;
                }
            } catch(const std::exception &e) {
                string rep = U("Score submission failed.");
                message.reply(status_codes::InternalError, rep).wait();
                std::cerr << "Score submission: Exception occurred:\n\t";
                std::cerr << e.what() << std::endl;
            }
        }).wait();
    } else if (message.relative_uri().path() == "/scores/rerank") {
        try {
            updateRanks();
            message.reply(status_codes::OK, U("Score rerank successful.")).wait();
        } catch(const std::exception &e) {
            string rep = U("Score rerank failed.");
            message.reply(status_codes::InternalError, rep).wait();
            std::cerr << "Score rerank: Exception occurred:\n\t";
            std::cerr << e.what() << std::endl;
        }
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

                _results.rankings.resize(_schedule.phases.size());

                string rep = U("Schedule loading successful.");
                message.reply(status_codes::OK, rep).wait();
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

void HTTPHandler::updateRanks() {
    for_each(_teams.begin(), _teams.end(),
             [=](Team &team){
                 json response = _js.callFunction("GetTeamScore",{team.toJSON(),_schedule.currentPhase});
                 team.displayScore = response["score"];
             });
    stable_sort(_teams.begin(), _teams.end(),
                [=](const Team &a, const Team &b){
                    json response = _js.callFunction("CompareTeams",{a.toJSON(),b.toJSON()});
                    return response["result"];
                });
    int rank = 1;
    _teams[0].rank=rank;
    for(size_t i = 1; i < _teams.size(); i++) {
        if(_js.callFunction("CompareTeams",{_teams[i-1].toJSON(),_teams[i].toJSON()})["result"]) {
            rank++;
        }
        _teams[i].rank = rank;
    }
}

void HTTPHandler::updateResults() {
    auto currentPhaseResults = _results.rankings[_schedule.currentPhase];

    currentPhaseResults.clear();

    auto teamNumbers = _schedule.getCurrentPhase().getInvolvedTeamNumbers();

    std::vector<Team> teams = getTeamsFromNumbers(teamNumbers.begin(), teamNumbers.end());

    std::transform(teams.begin(), teams.end(), std::back_inserter(currentPhaseResults), [](const auto  &team){
        return Ranking{team.rank, team.number};
    });
}

void HTTPHandler::loadFunctionsFromJS(const std::string &scriptName) {
    ifstream fileIn{"resources/dynamic/" + competitionName + "/scripts/" + scriptName};
    string fileContents{istreambuf_iterator<char>{fileIn}, istreambuf_iterator<char>{}};
    _js.loadFunctionsFromString(fileContents);
}

void HTTPHandler::fillNextPhase() {
    auto currentPhase = _schedule.currentPhase;
    std::set<std::string> nextPhaseTeamKeys;
    auto nextPhase = _schedule.phases[currentPhase+1];

    for(const auto &match : nextPhase.matches) {
        for(const auto &team : match.team_numbers) {
            nextPhaseTeamKeys.insert(team);
        }
    }

    json nextPhaseMap = json::array();

    for(const auto &teamKey : nextPhaseTeamKeys) {
        nextPhaseMap.push_back({teamKey,""});
    }

    auto results = _results.toJSON();

    auto ret = _js.callFunction("SelectNextPhase", {results,currentPhase,nextPhaseMap});

    std::map<std::string, std::string> nextPhaseReplacementMap;


}
