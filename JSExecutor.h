#ifndef OLYMPUS_JSEXECUTOR_H
#define OLYMPUS_JSEXECUTOR_H

#include <string>
#include "duktape/duktape.h"
#include "json.hpp"

using json = nlohmann::json;

class JSExecutor {
public:
    JSExecutor();

    ~JSExecutor();

    void loadFunctionsFromString(std::string source);

    json callFunction(std::string functionName, std::vector<json> arguments);

private:

    duk_context *_context;
};


#endif //OLYMPUS_JSEXECUTOR_H
