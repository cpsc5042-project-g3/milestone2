cmake_minimum_required(VERSION 3.17)
project(Server)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES ServerMain.cpp Server.h Server.cpp RPCImpl.h RPCImpl.cpp Character.h Character.cpp Game.h Game.cpp)

find_package( Threads )
add_executable(Server ${SOURCE_FILES})

target_link_libraries( Server ${CMAKE_THREAD_LIBS_INIT})