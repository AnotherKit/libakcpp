cmake_minimum_required(VERSION 3.16)
project(libakcpp)
include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Ofast")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(AKCPP_SOURCES
  src/ak/base.cpp
  src/ak/validator/_internals/common.cpp
  src/ak/validator/_internals/string.cpp
)

add_library(akcpp STATIC ${AKCPP_SOURCES})
add_library(akcppso SHARED ${AKCPP_SOURCES})
set_target_properties(akcppso PROPERTIES OUTPUT_NAME akcpp)

enable_testing()
set(AKCPP_TEST_SOURCES
  src/ak/compare_test.cpp
  src/ak/validator_test.cpp
  src/ak/validator/_internals/string_test.cpp
)

foreach(test ${AKCPP_TEST_SOURCES})
  string(MD5 testmd5 ${test})
  string(SUBSTRING ${testmd5} 0 6 testmd5)
  get_filename_component(TName ${test} NAME_WE)
  add_executable(${testmd5}-${TName} ${test})
  target_link_libraries(${testmd5}-${TName} akcpp)
  add_test(NAME ${testmd5}-${TName} COMMAND ${testmd5}-${TName})
endforeach()
