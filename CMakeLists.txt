cmake_minimum_required(VERSION 3.10)
project(HighPerformanceServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES "src/net/*.cpp")

add_executable(my_server examples/main.cpp ${SOURCES})

target_link_libraries(my_server pthread)