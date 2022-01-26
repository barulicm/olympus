#include <iostream>
#include <filesystem>
#include "http_listener.h"
#include "request_handler.h"
#include "request_handlers/AllRequestHandlers.h"
#include "common/session.h"
#include "javascript_executor.h"

std::vector<std::string> getDirectories(const std::string& root) {
    std::vector<std::string> directories;
    for(const auto& dir_entry : std::filesystem::directory_iterator(std::filesystem::path(root))) {
        if(dir_entry.is_directory()) {
            directories.push_back(dir_entry.path().filename().string());
        }
    }
    return directories;
}

void InitializeSession(Session& session) {
    std::cout << "New session? [(y)/n]:\n";
    auto newSessionSelection = '\0';
    std::cin >> newSessionSelection;
    if(newSessionSelection == 'y') {
        std::cout << "Choose a competition:\n";
        auto competitions = getDirectories("resources/dynamic");
        auto i = 0;
        for(const auto &competition : competitions) {
            std::cout << i << " - " << competition << "\n";
            i++;
        }
        size_t competitionIndexIn;
        std::cin >> competitionIndexIn;
        competitionIndexIn = std::min(competitionIndexIn, competitions.size());
        session.competition_name = competitions[competitionIndexIn];
    } else {
        std::cout << "Please provide a session file to load:\n";
        std::string sessionFilePath;
        std::cin >> sessionFilePath;
//        httpHandler.loadSession(sessionFilePath);
    }
}

nlohmann::json loadDefaultCustomFields(JavascriptExecutor& javascript_executor) {
    try {
        return javascript_executor.callFunction("DefaultCustomFields",{});
    } catch (std::exception &e) {
        std::cerr << "Error loading default custom fields:" << std::endl;
        std::cerr << e.what() << std::endl;
        return {};
    }
}

void loadFunctionsFromJS(JavascriptExecutor& javascript_executor, const std::string& competitionName, const std::string &scriptName) {
    std::ifstream fileIn{"resources/dynamic/" + competitionName + "/scripts/" + scriptName};
    std::string fileContents{std::istreambuf_iterator<char>{fileIn}, std::istreambuf_iterator<char>{}};
    javascript_executor.loadFunctionsFromString(fileContents);
}

void LoadAllCompetitionFunctions(JavascriptExecutor& javascript_executor, const std::string& competitionName) {
    loadFunctionsFromJS(javascript_executor, competitionName, "CompareTeams.js");
    loadFunctionsFromJS(javascript_executor, competitionName, "GetTeamScore.js");
    loadFunctionsFromJS(javascript_executor, competitionName, "SelectNextPhase.js");
    loadFunctionsFromJS(javascript_executor, competitionName, "DefaultCustomFields.js");
}

int main(int argc, char *argv[]) {
//    auto url = U("http://127.0.0.1:8080");
    const auto url = U("http://0.0.0.0:8080"); // listens on all interfaces on Linux
    web::uri_builder uri{url};
    const auto address = uri.to_uri().to_string();

    Session session;
    InitializeSession(session);

    JavascriptExecutor javascript_executor;
    LoadAllCompetitionFunctions(javascript_executor, session.competition_name);
    nlohmann::json defaultCustomTeamFields = loadDefaultCustomFields(javascript_executor);

    HttpListener httpHandler;
    httpHandler.setUrl(address);

    std::vector<std::unique_ptr<RequestHandler>> requestHandlers;
    requestHandlers.push_back(std::make_unique<TeamHandler>(session, defaultCustomTeamFields));
    requestHandlers.push_back(std::make_unique<TimerHandler>());
    requestHandlers.push_back(std::make_unique<ScoresHandler>(session, javascript_executor));
    requestHandlers.push_back(std::make_unique<ScheduleHandler>(session, javascript_executor));
    requestHandlers.push_back(std::make_unique<ResultsHandler>(session));
    requestHandlers.push_back(std::make_unique<ControlQueryHandler>(session));
    requestHandlers.push_back(std::make_unique<DynamicResourceHandler>(session.competition_name));
    requestHandlers.push_back(std::make_unique<StaticResourceHandler>());

    for(auto& handler : requestHandlers) {
        for(const auto& details : handler->GetHandlers()) {
            httpHandler.registerRequestHandler(details);
        }
    }

    httpHandler.open().wait();

    std::cout << "Listening for requests at: " << address << "\n";
    std::cout << "Press ENTER to exit.\n";

    std::cin.ignore();
    std::string line;
    std::getline(std::cin, line);

    httpHandler.close().wait();

    return 0;
}