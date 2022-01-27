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
  src/ak/chalk.cpp
  src/ak/validator/_internals/common.cpp
  src/ak/validator/_internals/string.cpp
)

add_library(akcpp STATIC ${AKCPP_SOURCES})
add_library(akcppso SHARED ${AKCPP_SOURCES})
target_include_directories(akcpp PRIVATE ${libakcpp_SOURCE_DIR}/include)
target_include_directories(akcppso PRIVATE ${libakcpp_SOURCE_DIR}/include)
set_target_properties(akcppso PROPERTIES OUTPUT_NAME akcpp)

enable_testing()
set(AKCPP_TEST_SOURCES
  src/ak/compare_test.cpp
  src/ak/chalk_test.cpp
  src/ak/validator_test.cpp
  src/ak/validator/_internals/string_test.cpp
)

foreach(test ${AKCPP_TEST_SOURCES})
  string(MD5 testmd5 ${test})
  string(SUBSTRING ${testmd5} 0 6 testmd5)
  get_filename_component(TName ${test} NAME_WE)
  set(testexe ${testmd5}-${TName})
  add_executable(${testexe} ${test})
  target_include_directories(${testexe} PRIVATE ${libakcpp_SOURCE_DIR}/include)
  target_link_libraries(${testexe} akcpp)
  add_test(NAME ${testexe} COMMAND ${testexe})
endforeach()
