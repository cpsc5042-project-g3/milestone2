cmake_minimum_required(VERSION 3.17)
project(Server)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp Server.h Server.cpp)

add_executable(Server ${SOURCE_FILES})