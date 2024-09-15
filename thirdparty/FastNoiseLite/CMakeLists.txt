cmake_minimum_required(VERSION 3.13)

project(FastNoiseLite)

add_library(FastNoiseLite INTERFACE)
target_include_directories(FastNoiseLite INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")