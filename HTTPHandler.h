#ifndef OLYMPUS_HANDLER_H
#define OLYMPUS_HANDLER_H

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>
#include <cpprest/producerconsumerstream.h>
#include "common/Team.h"
#include "JSExecutor.h"
#include "common/Schedule.h"
#include "common/Results.h"

class HTTPHandler {
public:
    HTTPHandler();
    virtual ~HTTPHandler() = default;

    static std::unique_ptr<HTTPHandler> fromUrlAndCompetitionName(utility::string_t url,
                                                                  const utility::string_t &competitionName);
    static std::unique_ptr<HTTPHandler> fromUrlAndSavedSession(utility::string_t url,
                                                               const utility::string_t &sessionFilePath);

    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

    void setUrl(const utility::string_t &url);
    void setCompetitionName(const utility::string_t &name);

    void saveSession(const std::string &path);
    void loadSession(const std::string &path);

private:
    std::vector<Team> _teams;

    Schedule _schedule;

    TournamentResults _results;

    JSExecutor _js;

    nlohmann::json _defaultCustomFields;

    void handle_get(web::http::http_request message);
    void handle_put(web::http::http_request message);
    void handle_post(web::http::http_request message);
    void handle_delete(web::http::http_request message);
    web::http::experimental::listener::http_listener m_listener;

    bool file_exists(std::string filename);

    std::string mime_type_for_path(std::string path);

    void updateRanks();

    void updateResults();

    void loadFunctionsFromJS(const std::string &scriptName);

    void fillNextPhase();

    void loadDefaultCustomFields();

    template<typename InputIterator>
    std::vector<Team> getTeamsFromNumbers(InputIterator first, InputIterator last);

    std::map<std::string,std::string> _mime_types;

    utility::string_t competitionName;
};

template<typename InputIterator>
std::vector<Team> HTTPHandler::getTeamsFromNumbers(InputIterator first, InputIterator last) {
    std::vector<Team> teams;
    std::transform(first, last, std::back_inserter(teams), [this](const auto &teamNumber){
        auto find_res = std::find_if(_teams.begin(), _teams.end(), [&teamNumber](const auto &team){ return team.number == teamNumber; });
        if(find_res != _teams.end()) {
            return Team(*find_res);
        } else {
            return Team();
        }
    });
    return teams;
}


#endif //OLYMPUS_HANDLER_H
