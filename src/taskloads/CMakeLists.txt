aux_source_directory(. DIR_TASKLOADS_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -g -O0 -std=c++11 -I ../")
SET ( CMAKE_EXE_LINKER_FLAGS  "-lpthread -lm" )
add_library(taskloads STATIC ${DIR_TASKLOADS_SRCS})
