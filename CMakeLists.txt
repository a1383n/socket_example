cmake_minimum_required(VERSION 3.27)
project(socket_example C)

set(CMAKE_C_STANDARD 11)

add_executable(socket_example main.c tcp.c)
