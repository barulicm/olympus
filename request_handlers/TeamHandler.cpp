#include <filesystem>
#include "TeamHandler.h"

TeamHandler::TeamHandler(Session &session, nlohmann::json default_custom_fields)
    : session_(session),
      _defaultCustomFields(std::move(default_custom_fields))
{

}

std::vector<RequestHandlerDetails> TeamHandler::GetHandlers() {
    auto path_predicate = [](const utility::string_t& path){
        return path.starts_with("/team/");
    };
    return {{
                    web::http::methods::GET,
                    path_predicate,
                    std::bind_front(&TeamHandler::CallbackGet, this)
            },
            {
                    web::http::methods::PUT,
                    path_predicate,
                    std::bind_front(&TeamHandler::CallbackPut, this)
            }};
}

void TeamHandler::CallbackGet(const web::http::http_request &request) {
    auto team_number = std::filesystem::path{request.relative_uri().path()}.filename();
    if(team_number == "all") {
        nlohmann::json j = nlohmann::json::array();
        for(const auto &team : session_._teams) {
            j.push_back(team.toJSON());
        }
        request.reply(web::http::status_codes::OK, j.dump(), U("application/json")).wait();
    } else if (team_number == "active") {
        nlohmann::json j = nlohmann::json::array();
        if(session_._schedule.isValid()) {
            auto activeTeamNumbers = session_._schedule.getCurrentPhase().getInvolvedTeamNumbers();
            for(const auto& number : activeTeamNumbers) {
                j.push_back(GetTeamByNumber(number).toJSON());
            }
        }
        request.reply(web::http::status_codes::OK, j.dump(), U("application/json")).wait();
    } else {
        auto team_iter = std::find_if(session_._teams.begin(), session_._teams.end(),
                                      [&team_number](const Team &t){
                                          return t.number == team_number;
                                      });
        if(team_iter != session_._teams.end()) {
            request.reply(web::http::status_codes::OK, team_iter->toJSON().dump(), U("application/json")).wait();
        } else {
            request.reply(web::http::status_codes::NotFound, U("No such team")).wait();
        }
    }
}

void TeamHandler::CallbackPut(web::http::http_request request) {
    const auto path = request.relative_uri().path();
    if(path == "/team/add") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                Team newTeam;
                newTeam.customFields = _defaultCustomFields;
                newTeam.rank = 0;
                newTeam.number = j["number"];
                newTeam.name = j["name"];
                session_._teams.push_back(newTeam);
                std::string rep = U("Add team successful.");
                request.reply(web::http::status_codes::OK, rep).wait();
            } catch(...) {
                std::string rep = U("Add team failed.");
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(path == "/team/remove") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                std::string teamNumber = j["number"];
                auto findIter = std::find_if(session_._teams.begin(), session_._teams.end(), [&teamNumber](const auto &team){
                    return team.number == teamNumber;
                });
                if(findIter == session_._teams.end()) {
                    std::string rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {
                    session_._teams.erase(findIter);
                    std::string rep = U("Remove team successful.");
                    request.reply(web::http::status_codes::OK, rep).wait();
                }
            } catch(...) {
                std::string rep = U("Remove team failed.");
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(path == "/team/customFields") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                auto teamNumber = j["teamNumber"];
                auto findIter = std::find_if(session_._teams.begin(), session_._teams.end(), [&teamNumber](const auto &team) {
                    return team.number == teamNumber;
                });
                if(findIter == session_._teams.end()) {
                    std::string rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {
                    (*findIter).customFields = j["customFields"];
                    std::string rep = U("Setting custom fields successful.");
                    request.reply(web::http::status_codes::OK, rep).wait();
                }
            } catch(std::exception &e) {
                std::string rep = U(std::string("Setting custom fields failed.") + e.what());
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(path == "/team/edit") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                auto teamNumber = j["oldTeamNumber"];
                auto findIter = std::find_if(session_._teams.begin(), session_._teams.end(), [&teamNumber](const auto &team) {
                    return team.number == teamNumber;
                });
                if(findIter == session_._teams.end()) {
                    std::string rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {
                    findIter->number = j["newTeamNumber"];
                    findIter->name = j["newTeamName"];
                    auto &scores = findIter->scores[session_._schedule.currentPhase];
                    auto &newScores = j["newScores"];
                    for(auto i = 0; i < newScores.size(); i++) {
                        scores[i] = newScores[i];
                    }
                }
                std::string rep = U("Team edits saved.");
                request.reply(web::http::status_codes::OK, rep).wait();
            } catch(std::exception &e) {
                std::string rep = U(std::string("Editing team info failed.") + e.what());
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else {
        std::string rep = U("Resource not found.");
        request.reply(web::http::status_codes::NotFound, rep).wait();
    }
}

Team &TeamHandler::GetTeamByNumber(const std::string &number) {
    const auto found_iter = std::find_if(session_._teams.begin(), session_._teams.end(), [&number](const Team& team){
        return team.number == number;
    });
    if(found_iter == session_._teams.end()) {
        throw std::invalid_argument("No team found with number: " + number);
    }
    return *found_iter;
}
