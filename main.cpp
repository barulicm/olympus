#include <iostream>
#include "HTTPHandler.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

unique_ptr<HTTPHandler> g_httpHandler;

void on_initialize(const string_t &address) {
    uri_builder uri(address);

    auto addr = uri.to_uri().to_string();
    g_httpHandler = make_unique<HTTPHandler>(addr);
    g_httpHandler->open().wait();

    ucout << string_t(U("Listening for requests at: ")) << addr << endl;
}

void on_shutdown() {
    g_httpHandler->close().wait();
}

int main(int argc, char *argv[]) {

    on_initialize(U("http://localhost:34568"));

    cout << "Press ENTER to exit." << endl;

    string line;
    getline(cin, line);

    on_shutdown();

    return 0;
}