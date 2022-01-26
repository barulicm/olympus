#include <iostream>
#include "javascript_executor.h"

static void js_fatal_error_handler(void *, const char *msg) {
    throw std::runtime_error(msg);
}

JavascriptExecutor::JavascriptExecutor() {
    _context = duk_create_heap(nullptr, nullptr, nullptr, nullptr, js_fatal_error_handler);
}

JavascriptExecutor::~JavascriptExecutor() {
    duk_destroy_heap(_context);
}

/*
 * https://stackoverflow.com/questions/36446481/in-duktape-how-to-convert-a-js-source-file-content-to-bytecode
 */

void JavascriptExecutor::loadFunctionsFromString(std::string source) {
    duk_compile_string(_context,0,source.data());
    duk_dump_function(_context);
    duk_load_function(_context);
    duk_call(_context,0);
}

nlohmann::json JavascriptExecutor::callFunction(std::string functionName, std::vector<nlohmann::json> arguments) {
    duk_push_global_object(_context);
    duk_get_prop_string(_context,-1,functionName.data());
    duk_require_function(_context,-1);

    for(const auto &argument : arguments) {
        duk_push_string(_context, argument.dump().data());
        duk_json_decode(_context,-1);
    }

    duk_call(_context,static_cast<duk_idx_t>(arguments.size()));
    nlohmann::json j = nlohmann::json::parse(duk_json_encode(_context,-1));

    // pop return value and global object off duktape stack
    duk_pop_2(_context);

    return j;
}
