cmake_minimum_required(VERSION 3.17)
project(client)

set(CMAKE_CXX_STANDARD 11)

add_executable(client client.cpp Connect4.cpp)