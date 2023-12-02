################################################################################
# Preamble
cmake_minimum_required(VERSION 3.21)

project(
  aoc2023
  VERSION 3.2.0
  LANGUAGES CXX
)

message(STATUS "CMake build configuration for ${PROJECT_NAME} (${CMAKE_BUILD_TYPE}) ${PROJECT_VERSION}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

################################################################################
# targets

enable_testing()
add_subdirectory(src)

################################################################################