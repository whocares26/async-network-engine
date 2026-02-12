cmake_minimum_required(VERSION 3.10)
project(HighPerformanceServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(my_server 
    main.cpp 
    EventLoop.cpp
    InetAddress.cpp
    Socket.cpp
    Acceptor.cpp
    TcpConnection.cpp
    TcpServer.cpp
)