cmake_minimum_required(VERSION 3.16)
project(Olympus VERSION 2024.1)

set(CMAKE_CXX_STANDARD 23)

find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED)

include(cmake/CheckGit.cmake)
CheckGitSetup()

configure_file(src/version.hpp.in generated/version.hpp @ONLY)

add_library(olympus_library STATIC
        src/game_description/json_game.cpp
        src/game_description/json_mission.cpp
        src/state_description/json_session.cpp
        src/state_description/json_team.cpp
        src/request_handlers/config_handler.cpp
        src/request_handlers/control_query_handler.cpp
        src/request_handlers/scores_handler.cpp
        src/request_handlers/session_save_handler.cpp
        src/request_handlers/static_resources_handler.cpp
        src/request_handlers/team_handler.cpp
        src/request_handlers/timer_handler.cpp
        src/generation/scorecard_generation.cpp
        src/http_listener.cpp
        src/mime_types.cpp
        src/get_exec_path.cpp
        src/get_ip_addresses.cpp
        src/save_session_backup.cpp
        src/request_handlers/generated_resource_handler.cpp
        src/request_handlers/game_definition_handler.cpp
        src/generation/score_calculator_generation.cpp
        src/request_handlers/announcement_handler.cpp)
target_include_directories(olympus_library PUBLIC src)
target_include_directories(olympus_library PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_link_libraries(olympus_library PUBLIC
    Boost::boost
    cpprestsdk::cpprest
    OpenSSL::SSL
    nlohmann_json::nlohmann_json
        git_version
)

if(WIN32)
    target_link_libraries(olympus_library PUBLIC
            iphlpapi.lib
            Ws2_32.lib
            cpprestsdk::cpprestsdk_zlib_internal
            cpprestsdk::cpprestsdk_brotli_internal)
    target_compile_definitions(olympus_library PUBLIC _WIN32_WINNT=0x0A00)
endif()

add_executable(olympus_executable
    resources.rc
    src/main.cpp
)
set_target_properties(olympus_executable PROPERTIES OUTPUT_NAME olympus)
target_link_libraries(olympus_executable PRIVATE olympus_library)

configure_file(Olympus.ico ${CMAKE_CURRENT_BINARY_DIR}/Olympus.ico COPYONLY)

include(GNUInstallDirs)

install(DIRECTORY static
        DESTINATION ${CMAKE_INSTALL_DATADIR}/olympus)

install(DIRECTORY game_configs
        DESTINATION ${CMAKE_INSTALL_DATADIR}/olympus)

install(TARGETS olympus_executable
        DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES $<TARGET_RUNTIME_DLLS:olympus_executable> DESTINATION ${CMAKE_INSTALL_BINDIR})

if(TEST_OLYMPUS OR CMAKE_SOURCE_DIR STREQUAL Olympus_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(packaging)
