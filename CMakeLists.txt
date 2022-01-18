cmake_minimum_required(VERSION 3.16)
project(Olympus)

set(CMAKE_CXX_STANDARD 20)

find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(Olympus
    main.cpp
    HTTPHandler.cpp
    common/Schedule.cpp
    common/Match.cpp
    common/Team.cpp
    common/Phase.cpp
    common/Results.cpp
    duktape/duktape.c
    JSExecutor.cpp
)
target_link_libraries(Olympus
    cpprestsdk::cpprest
    OpenSSL::SSL
    Threads::Threads
)

file(COPY static DESTINATION resources)
file(COPY dynamic DESTINATION resources)

file(INSTALL static DESTINATION resources)
file(INSTALL dynamic DESTINATION resources)
