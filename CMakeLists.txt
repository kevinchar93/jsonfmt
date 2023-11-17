cmake_minimum_required(VERSION 3.26)
project(jsonfmt C)

set(PROJECT_DESCRIPTION "simple configurable JSON formatter")
message(STATUS "running CMake for ${PROJECT_NAME} - (${PROJECT_DESCRIPTION})")

set(CMAKE_C_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(src/lib)

add_subdirectory(src)
add_subdirectory(test)