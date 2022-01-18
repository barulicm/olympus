#include <iostream>
#include <filesystem>
#include "HTTPHandler.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

std::vector<std::string> getDirectories(const std::string& root) {
    std::vector<std::string> directories;
    for(const auto& dir_entry : std::filesystem::directory_iterator(std::filesystem::path(root))) {
        if(dir_entry.is_directory()) {
            directories.push_back(dir_entry.path().filename().string());
        }
    }
    return directories;
}

int main(int argc, char *argv[]) {

//    auto url = U("http://127.0.0.1:8080");
    auto url = U("http://0.0.0.0:8080"); // listens on all interfaces on Linux
    uri_builder uri{url};
    auto address = uri.to_uri().to_string();

    std::unique_ptr<HTTPHandler> httpHandler;

    cout << "New session? [(y)/n]:\n";
    auto newSessionSelection = '\0';
    cin >> newSessionSelection;
    if(newSessionSelection == 'y') {
        cout << "Choose a competition:\n";
        auto competitions = getDirectories("resources/dynamic");
        auto i = 0;
        for(const auto &competition : competitions) {
            cout << i << " - " << competition << "\n";
            i++;
        }
        size_t competitionIndexIn;
        cin >> competitionIndexIn;
        competitionIndexIn = min(competitionIndexIn, competitions.size());
        httpHandler = HTTPHandler::fromUrlAndCompetitionName(address, competitions[competitionIndexIn]);
    } else {
        cout << "Please provide a session file to load:\n";
        std::string sessionFilePath;
        cin >> sessionFilePath;
        httpHandler = HTTPHandler::fromUrlAndSavedSession(address, sessionFilePath);
    }

    httpHandler->open().wait();

    cout << "Listening for requests at: " << address << endl;
    cout << "Press ENTER to exit." << endl;

    cin.ignore();
    string line;
    getline(cin, line);

    httpHandler->close().wait();

    return 0;
}