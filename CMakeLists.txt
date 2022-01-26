cmake_minimum_required(VERSION 3.16)
project(Olympus)

set(CMAKE_CXX_STANDARD 20)

find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(Olympus
    main.cpp
    MimeTypes.cpp
    HTTPHandler.cpp
    common/Schedule.cpp
    common/Match.cpp
    common/Team.cpp
    common/Phase.cpp
    common/Results.cpp
    duktape/duktape.c
    request_handlers/ControlQueryHandler.cpp
    request_handlers/DynamicResourceHandler.cpp
    request_handlers/ResultsHandler.cpp
    request_handlers/ScheduleHandler.cpp
    request_handlers/ScoresHandler.cpp
    request_handlers/StaticResourceHandler.cpp
    request_handlers/TeamHandler.cpp
    request_handlers/TimerHandler.cpp
    JSExecutor.cpp
)
target_include_directories(Olympus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Olympus
    cpprestsdk::cpprest
    OpenSSL::SSL
    Threads::Threads
)

file(COPY static DESTINATION resources)
file(COPY dynamic DESTINATION resources)

file(INSTALL static DESTINATION resources)
file(INSTALL dynamic DESTINATION resources)
