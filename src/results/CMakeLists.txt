aux_source_directory(. DIR_RESULTS_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -std=c++11 -I ../")
SET ( CMAKE_EXE_LINKER_FLAGS  "-lpthread -lm" )
add_library(results ${DIR_RESULTS_SRCS})
