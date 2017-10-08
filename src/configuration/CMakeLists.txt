aux_source_directory(. DIR_CONFIG_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -g -O0 -std=c++11 -I ../")
SET ( CMAKE_EXE_LINKER_FLAGS  "-lpthread -lm" )
add_library(configuration STATIC ${DIR_CONFIG_SRCS})
