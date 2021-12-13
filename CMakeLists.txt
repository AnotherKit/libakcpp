cmake_minimum_required(VERSION 3.16)
project(libakcpp)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_library(akcpp STATIC src/ak/base.cpp)
add_library(akcppso SHARED src/ak/base.cpp)
set_target_properties(akcppso PROPERTIES OUTPUT_NAME akcpp)
