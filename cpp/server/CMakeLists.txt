cmake_minimum_required(VERSION 3.17)
project(server)

set(CMAKE_CXX_STANDARD 11)

add_executable(server main.cpp RPCServer.cpp Connect4.cpp)

set(CMAKE_CXX_FLAGS -pthread)