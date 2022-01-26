cmake_minimum_required(VERSION 3.16)
project(Olympus)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Duktape REQUIRED IMPORTED_TARGET duktape)

find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(Olympus
    main.cpp
    mime_types.cpp
    http_listener.cpp
    common/schedule.cpp
    common/match.cpp
    common/team.cpp
    common/phase.cpp
    common/results.cpp
    request_handlers/ControlQueryHandler.cpp
    request_handlers/DynamicResourceHandler.cpp
    request_handlers/ResultsHandler.cpp
    request_handlers/ScheduleHandler.cpp
    request_handlers/ScoresHandler.cpp
    request_handlers/StaticResourceHandler.cpp
    request_handlers/TeamHandler.cpp
    request_handlers/TimerHandler.cpp
    javascript_executor.cpp
)
target_include_directories(Olympus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Olympus
    cpprestsdk::cpprest
    OpenSSL::SSL
    Threads::Threads
    nlohmann_json::nlohmann_json
    PkgConfig::Duktape
)

file(COPY static DESTINATION resources)
file(COPY dynamic DESTINATION resources)

file(INSTALL static DESTINATION resources)
file(INSTALL dynamic DESTINATION resources)
