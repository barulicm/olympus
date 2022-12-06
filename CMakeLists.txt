cmake_minimum_required(VERSION 3.16)
project(Olympus)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Duktape REQUIRED IMPORTED_TARGET duktape)

find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(olympus_library
        src/json_mission.cpp
        src/json_team.cpp
        src/json_session.cpp
        src/http_listener.cpp
        src/mime_types.cpp
        src/request_handlers/config_handler.cpp
        src/request_handlers/control_query_handler.cpp
        src/request_handlers/results_handler.cpp
        src/request_handlers/scores_handler.cpp
        src/request_handlers/session_save_handler.cpp
        src/request_handlers/static_resources_handler.cpp
        src/request_handlers/team_handler.cpp
        src/request_handlers/timer_handler.cpp
)
target_include_directories(olympus_library PUBLIC src)
target_link_libraries(olympus_library PUBLIC
    cpprestsdk::cpprest
    OpenSSL::SSL
    nlohmann_json::nlohmann_json
    PkgConfig::Duktape
)

add_executable(olympus_executable
    src/main.cpp
)
target_link_libraries(olympus_executable PRIVATE olympus_library)

add_executable(Olympus
    main.cpp
    mime_types.cpp
    http_listener.cpp
    common/schedule.cpp
    common/session.cpp
    common/config.cpp
    common/match.cpp
    common/team.cpp
    common/phase.cpp
    common/results.cpp
    request_handlers/config_handler.cpp
    request_handlers/control_query_handler.cpp
    request_handlers/dynamic_resource_handler.cpp
    request_handlers/results_handler.cpp
    request_handlers/schedule_handler.cpp
    request_handlers/scores_handler.cpp
    request_handlers/session_save_handler.cpp
    request_handlers/static_resources_handler.cpp
    request_handlers/team_handler.cpp
    request_handlers/timer_handler.cpp
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

if(TEST_OLYMPUS OR CMAKE_SOURCE_DIR STREQUAL Olympus_SOURCE_DIR)
    add_subdirectory(test)
endif()
