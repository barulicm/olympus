#include <filesystem>
#include "team_handler.hpp"
#include <nlohmann/json.hpp>
#include "state_description/json_team.hpp"

TeamHandler::TeamHandler(Session &session)
    : session_(session)
{

}

std::vector<RequestHandlerDetails> TeamHandler::GetHandlers() {
    auto path_predicate = [](const utility::string_t& path){
        return path.starts_with(U("/team/"));
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
        const auto json_dump = j.dump();
        request.reply(web::http::status_codes::OK, utility::string_t(json_dump.begin(), json_dump.end()), U("application/json")).wait();
    } else {
        auto team_iter = std::find_if(session_.teams.begin(), session_.teams.end(),
                                      [&team_number](const Team &t){
                                          return t.number == team_number;
                                      });
        if(team_iter != session_.teams.end()) {
            const auto json_dump = nlohmann::json(*team_iter).dump();
            request.reply(web::http::status_codes::OK, utility::string_t(json_dump.begin(), json_dump.end()), U("application/json")).wait();
        } else {
            request.reply(web::http::status_codes::NotFound, U("No such team")).wait();
        }
    }
}

void TeamHandler::CallbackPut(web::http::http_request request) {
    const auto path = request.relative_uri().path();
    if(path == U("/team/add")) {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                Team newTeam;
                newTeam.rank = 0;
                newTeam.number = j["number"];
                newTeam.name = j["name"];
                session_.teams.push_back(newTeam);
                utility::string_t rep = U("Add team successful.");
                request.reply(web::http::status_codes::OK, rep).wait();
            } catch(const std::exception& e) {
                const std::string error_msg = e.what();
                utility::string_t rep = U("Add team failed: ") + utility::string_t(error_msg.begin(), error_msg.end());
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(path == U("/team/remove")) {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                std::string teamNumber = j["number"];
                auto findIter = std::find_if(session_.teams.begin(), session_.teams.end(), [&teamNumber](const auto &team){
                    return team.number == teamNumber;
                });
                if(findIter == session_.teams.end()) {
                    utility::string_t rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {
                    session_.teams.erase(findIter);
                    utility::string_t rep = U("Remove team successful.");
                    request.reply(web::http::status_codes::OK, rep).wait();
                }
            } catch(...) {
                utility::string_t rep = U("Remove team failed.");
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else if(path == U("/team/edit")) {
        request.extract_string().then([this,&request](const utility::string_t& body){
            try {
                nlohmann::json j = nlohmann::json::parse(body);
                const std::string teamNumber = j["oldTeamNumber"];
                auto findIter = std::find_if(session_.teams.begin(), session_.teams.end(), [&teamNumber](const auto &team) {
                    return team.number == teamNumber;
                });
                if(findIter == session_.teams.end()) {
                    utility::string_t rep = U("No such team.");
                    request.reply(web::http::status_codes::NotFound, rep).wait();
                } else {

                    findIter->number = j["newTeamNumber"];
                    findIter->name = j["newTeamName"];
                    j["newScores"].get_to(findIter->scores);
                    j["newGPScores"].get_to(findIter->gp_scores);
                }
                utility::string_t rep = U("Team edits saved.");
                request.reply(web::http::status_codes::OK, rep).wait();
            } catch(std::exception &e) {
                const std::string error_msg = e.what();
                utility::string_t rep = U("Editing team info failed.") + utility::string_t(error_msg.begin(), error_msg.end());
                request.reply(web::http::status_codes::InternalError, rep).wait();
            }
        }).wait();
    } else {
        utility::string_t rep = U("Resource not found.");
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
