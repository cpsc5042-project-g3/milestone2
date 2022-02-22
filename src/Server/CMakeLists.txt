cmake_minimum_required(VERSION 3.17)
project(Server)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES ServerMain.cpp Server.h Server.cpp RPCImpl.h RPCImpl.cpp LocalContext.h Character.cpp Character.h)

add_executable(Server ${SOURCE_FILES})