#include <filesystem>
#include "team_handler.hpp"
#include <nlohmann/json.hpp>
#include "json_team.hpp"

TeamHandler::TeamHandler(Session &session)
    : session_(session)
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
        nlohmann::json j = session_.teams;
        request.reply(web::http::status_codes::OK, j.dump(), U("application/json")).wait();
    } else {
        auto team_iter = std::find_if(session_.teams.begin(), session_.teams.end(),
                                      [&team_number](const Team &t){
                                          return t.number == team_number;
                                      });
        if(team_iter != session_.teams.end()) {
            request.reply(web::http::status_codes::OK, nlohmann::json(*team_iter).dump(), U("application/json")).wait();
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
                newTeam.rank = 0;
                newTeam.number = j["number"];
                newTeam.name = j["name"];
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
                    return team.number == teamNumber;
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
    } else if(path == "/team/edit") {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                auto teamNumber = j["oldTeamNumber"];
                auto findIter = std::find_if(session_.teams.begin(), session_.teams.end(), [&teamNumber](const auto &team) {
                    return team.number == teamNumber;
                });
                if(findIter == session_.teams.end()) {
                    std::string rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {

                    findIter->number = j["newTeamNumber"];
                    findIter->name = j["newTeamName"];
                    j["newScores"].get_to(findIter->scores);
                    j["newGPScores"].get_to(findIter->gp_scores);
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
        return team.number == number;
    });
    if(found_iter == session_.teams.end()) {
        throw std::invalid_argument("No team found with number: " + number);
    }
    return *found_iter;
}
