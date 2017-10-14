cmake_minimum_required(VERSION 2.8)


project(McFTP)

INCLUDE_DIRECTORIES(./src/)
SET( CMAKE_CXX_FLAGS "-Wall -std=c++11 -g -O3")
SET( CMAKE_EXE_LINKER_FLAGS  " -lm -pthread" )

aux_source_directory(. DIR_SRCS)

add_subdirectory(./src)


add_executable(mcftp main.cpp)


target_link_libraries(mcftp  cmi)


add_executable(softSensorModel softSensorModelPrecompute.cpp)

target_link_libraries(softSensorModel  cmi)