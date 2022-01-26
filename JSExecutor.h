#ifndef OLYMPUS_JSEXECUTOR_H
#define OLYMPUS_JSEXECUTOR_H

#include <string>
#include "duktape/duktape.h"
#include <nlohmann/json.hpp>

class JSExecutor {
public:
    JSExecutor();

    ~JSExecutor();

    void loadFunctionsFromString(std::string source);

    nlohmann::json callFunction(std::string functionName, std::vector<nlohmann::json> arguments);

private:

    duk_context *_context;
};


#endif //OLYMPUS_JSEXECUTOR_H
