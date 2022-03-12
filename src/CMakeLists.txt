cmake_minimum_required(VERSION 3.17)

# Set the project name
project(Guess-Who-Game)
set(CMAKE_CXX_STANDARD 14)

find_package( Threads )

#Add source to this project's executable.
add_subdirectory(Client)
add_subdirectory(Server)


target_link_libraries( Client ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries( Server ${CMAKE_THREAD_LIBS_INIT})