cmake_minimum_required(VERSION 3.17)
project(Client)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES ClientMain.cpp Client.cpp Client.h)

add_executable(Client ${SOURCE_FILES})
