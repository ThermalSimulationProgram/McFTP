aux_source_directory(. DIR_PTHREAD_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -std=c++11 -I ../")
SET ( CMAKE_EXE_LINKER_FLAGS  "-lpthread -lm" )
add_library(pthread ${DIR_PTHREAD_SRCS})
