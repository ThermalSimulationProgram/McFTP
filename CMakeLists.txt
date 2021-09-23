cmake_minimum_required(VERSION 2.8)

set(HARD_TEMPERATURE_SENSOR_ENABLE "ON")
set(SOFT_TEMPERATURE_SENSOR_ENABLE "ON")

configure_file(./McFTPBuildConfig.h.in  ../McFTPBuildConfig.h @ONLY)



project(McFTP)

INCLUDE_DIRECTORIES(./src/)

SET( CMAKE_CXX_FLAGS "-Wall -std=c++11 -g -O3")
SET( CMAKE_EXE_LINKER_FLAGS  " -lm -pthread" )

aux_source_directory(. DIR_SRCS)

add_subdirectory(./src)

set(EXECUTABLE_OUTPUT_PATH ../)
add_executable(mcftp main.cpp)


target_link_libraries(mcftp  cmi)
