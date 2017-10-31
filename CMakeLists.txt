cmake_minimum_required(VERSION 3.5)
project(Olympus)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(${Boost_INCLUDE_DIRS} SYSTEM)

find_package(Casablanca REQUIRED)

include_directories(${CASABLANCA_INCLUDE_DIR} SYSTEM)

find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

set(SOURCE_FILES main.cpp HTTPHandler.cpp common/Schedule.cpp common/Match.cpp common/Team.cpp common/Phase.cpp common/Results.cpp duktape/duktape.c JSExecutor.cpp JSExecutor.h)
add_executable(Olympus ${SOURCE_FILES})
target_link_libraries(Olympus ${Boost_LIBRARIES} ${CASABLANCA_LIBRARY} ${OPENSSL_LIBRARIES})

file(COPY static DESTINATION resources)
file(COPY dynamic DESTINATION resources)

file(INSTALL static DESTINATION resources)
file(INSTALL dynamic DESTINATION resources)
