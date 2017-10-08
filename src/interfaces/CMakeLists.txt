aux_source_directory(. DIR_API_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -g -O3 -std=c++11 -I ../")
add_library(interfaces STATIC ${DIR_API_SRCS})

