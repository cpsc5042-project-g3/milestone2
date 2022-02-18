cmake_minimum_required(VERSION 3.19)
project(Test)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Test
        Server/ApprovedUsers.txt
        Client/Client.cpp
        Client/Client.h
        Client/ClientMain.cpp
        Server/Server.cpp
        Server/Server.h
        Server/ServerMain.cpp)
