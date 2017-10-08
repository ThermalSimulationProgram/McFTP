aux_source_directory(. DIR_DISP_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -g -O3 -std=c++11 -I ../")
add_library(dispatchers  STATIC ${DIR_DISP_SRCS})
