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

add_library(olympus_library STATIC
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
        src/get_exec_path.cpp
        src/get_ip_addresses.cpp)
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
set_target_properties(olympus_executable PROPERTIES OUTPUT_NAME olympus)
target_link_libraries(olympus_executable PRIVATE olympus_library)

include(GNUInstallDirs)

install(DIRECTORY static
        DESTINATION ${CMAKE_INSTALL_DATADIR}/olympus)

install(TARGETS olympus_executable
        DESTINATION ${CMAKE_INSTALL_BINDIR})

if(TEST_OLYMPUS OR CMAKE_SOURCE_DIR STREQUAL Olympus_SOURCE_DIR)
    add_subdirectory(test)
endif()
