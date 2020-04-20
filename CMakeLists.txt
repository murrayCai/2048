cmake_minimum_required(VERSION 3.16)
project(2048 C)

add_library(2048core c2048.c)
add_executable(color color.c)
add_executable(2048 2048.c)
target_link_libraries(2048 2048core)
