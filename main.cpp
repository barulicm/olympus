#include <iostream>
#include <boost/filesystem.hpp>
#include "HTTPHandler.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

unique_ptr<HTTPHandler> g_httpHandler;

void on_initialize(const string_t &address, const string &competition) {
    uri_builder uri(address);

    auto addr = uri.to_uri().to_string();
    g_httpHandler = make_unique<HTTPHandler>(addr, competition.c_str());
    g_httpHandler->open().wait();

    ucout << string_t(U("Listening for requests at: ")) << addr << endl;
}

void on_shutdown() {
    g_httpHandler->close().wait();
}

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
    }
    size_t competitionIndexIn;
    cin >> competitionIndexIn;
    competitionIndexIn = min(competitionIndexIn, competitions.size());

    on_initialize(U("http://localhost:34568"), competitions[competitionIndexIn]);

    cout << "Press ENTER to exit." << endl;

    cin.ignore();
    string line;
    getline(cin, line);

    on_shutdown();

    return 0;
}