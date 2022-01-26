#ifndef OLYMPUS_JAVASCRIPT_EXECUTOR_H
#define OLYMPUS_JAVASCRIPT_EXECUTOR_H

#include <string>
#include <duktape.h>
#include <nlohmann/json.hpp>

class JavascriptExecutor {
public:
    JavascriptExecutor();

    ~JavascriptExecutor();

    void loadFunctionsFromString(std::string source);

    nlohmann::json callFunction(std::string functionName, std::vector<nlohmann::json> arguments);

private:

    duk_context *_context;
};

#endif //OLYMPUS_JAVASCRIPT_EXECUTOR_H
