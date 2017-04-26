cmake_minimum_required(VERSION 2.8)

project(McFTP)

INCLUDE_DIRECTORIES(./src/)
SET( CMAKE_CXX_FLAGS "-Wall -std=c++11 -g -O3")
SET( CMAKE_EXE_LINKER_FLAGS  "-pthread -lm" )

aux_source_directory(. DIR_SRCS)

add_subdirectory(./src/interfaces)
add_subdirectory(./src/benchmarks)
add_subdirectory(./src/UnitTest)
add_subdirectory(./src/configuration)
add_subdirectory(./src/core)
add_subdirectory(./src/dispatchers)
add_subdirectory(./src/pthread)
add_subdirectory(./src/results)
add_subdirectory(./src/tasks)
add_subdirectory(./src/taskloads)
add_subdirectory(./src/utils)

add_executable(mcftp main.cpp)


target_link_libraries(mcftp UnitTest core interfaces configuration  dispatchers pthread results tasks taskloads utils benchmarks)