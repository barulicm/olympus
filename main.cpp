#include <iostream>
#include <boost/filesystem.hpp>
#include "HTTPHandler.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

vector<string> getDirectories(string root) {
    using namespace boost::filesystem;
    path p{root};
    vector<string> directories;
    for(directory_iterator itr{p}; itr != directory_iterator{}; itr++) {
        if(is_directory(itr->path())) {
            directories.push_back(itr->path().leaf().string());
        }
    }
    return directories;
}

int main(int argc, char *argv[]) {

    auto competitions = getDirectories("resources/dynamic");

    cout << "Choose a competition:\n";
    auto i = 0;
    for(const auto &competition : competitions) {
        cout << i << " - " << competition << "\n";
        i++;
    }
    size_t competitionIndexIn;
    cin >> competitionIndexIn;
    competitionIndexIn = min(competitionIndexIn, competitions.size());

    auto url = U("http://127.0.0.1:8080");
//    auto url = U("http://192.168.1.20:8080");

    uri_builder uri{url};
    auto address = uri.to_uri().to_string();
    auto httpHandler = HTTPHandler::fromUrlAndCompetitionName(address, competitions[competitionIndexIn]);
    httpHandler->open().wait();

    cout << "Listening for requests at: " << address << endl;
    cout << "Press ENTER to exit." << endl;

    cin.ignore();
    string line;
    getline(cin, line);

    httpHandler->close().wait();

    return 0;
}