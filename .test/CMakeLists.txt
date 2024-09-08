cmake_minimum_required(VERSION 3.10)
project(algo_test VERSION 0.1.0 LANGUAGES C)

set (CMAKE_CXX_FLAGS “${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Wno-unknown-pragmas -fdiagnostics-color=always -std=c11”)

include(CTest)
enable_testing()

add_executable(algo_test test.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
