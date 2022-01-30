#include <filesystem>
#include "team_handler.h"

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
        for(const auto &team : session_.teams) {
            j.push_back(team.ToJson());
        }
        request.reply(web::http::status_codes::OK, j.dump(), U("application/json")).wait();
    } else if (team_number == "active") {
        nlohmann::json j = nlohmann::json::array();
        if(session_.schedule.IsValid()) {
            auto activeTeamNumbers = session_.schedule.GetCurrentPhase().GetInvolvedTeamNumbers();
            for(const auto& number : activeTeamNumbers) {
                j.push_back(GetTeamByNumber(number).ToJson());
            }
        }
        request.reply(web::http::status_codes::OK, j.dump(), U("application/json")).wait();
    } else {
        auto team_iter = std::find_if(session_.teams.begin(), session_.teams.end(),
                                      [&team_number](const Team &t){
                                          return t.number_ == team_number;
                                      });
        if(team_iter != session_.teams.end()) {
            request.reply(web::http::status_codes::OK, team_iter->ToJson().dump(), U("application/json")).wait();
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
                newTeam.custom_fields_ = _defaultCustomFields;
                newTeam.rank_ = 0;
                newTeam.number_ = j["number"];
                newTeam.name_ = j["name"];
                session_.teams.push_back(newTeam);
                std::string rep = U("Add team successful.");
                request.reply(web::http::status_codes::OK, rep).wait();
            } catch(const std::exception& e) {
                std::string rep = U(std::string("Add team failed: ") + e.what());
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(path == "/team/remove") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                std::string teamNumber = j["number"];
                auto findIter = std::find_if(session_.teams.begin(), session_.teams.end(), [&teamNumber](const auto &team){
                    return team.number_ == teamNumber;
                });
                if(findIter == session_.teams.end()) {
                    std::string rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {
                    session_.teams.erase(findIter);
                    std::string rep = U("Remove team successful.");
                    request.reply(web::http::status_codes::OK, rep).wait();
                }
            } catch(...) {
                std::string rep = U("Remove team failed.");
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(path == "/team/custom_fields_") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                auto teamNumber = j["team_number"];
                auto findIter = std::find_if(session_.teams.begin(), session_.teams.end(), [&teamNumber](const auto &team) {
                    return team.number_ == teamNumber;
                });
                if(findIter == session_.teams.end()) {
                    std::string rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {
                    (*findIter).custom_fields_ = j["custom_fields_"];
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
                auto findIter = std::find_if(session_.teams.begin(), session_.teams.end(), [&teamNumber](const auto &team) {
                    return team.number_ == teamNumber;
                });
                if(findIter == session_.teams.end()) {
                    std::string rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {
                    findIter->number_ = j["newTeamNumber"];
                    findIter->name_ = j["newTeamName"];
                    auto &scores = findIter->scores_[session_.schedule.current_phase];
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
    const auto found_iter = std::find_if(session_.teams.begin(), session_.teams.end(), [&number](const Team& team){
        return team.number_ == number;
    });
    if(found_iter == session_.teams.end()) {
        throw std::invalid_argument("No team found with number: " + number);
    }
    return *found_iter;
}
