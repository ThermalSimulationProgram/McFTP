aux_source_directory(. DIR_TASKS_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -g -O3 -std=c++11 -I ../")
SET ( CMAKE_EXE_LINKER_FLAGS  "-lpthread -lm" )
add_library(tasks STATIC ${DIR_TASKS_SRCS})
