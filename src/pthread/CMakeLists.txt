aux_source_directory(. DIR_PTHREAD_SRCS)
set (CMAKE_CXX_FLAGS "-Wall  -g -O3 -std=c++11 -I ../")
SET ( CMAKE_EXE_LINKER_FLAGS  "-lpthread -lm" )
add_library(pthread STATIC ${DIR_PTHREAD_SRCS})
