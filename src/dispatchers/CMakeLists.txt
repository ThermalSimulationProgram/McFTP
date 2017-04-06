aux_source_directory(. DIR_DISP_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -std=c++11 -I ../")
add_library(dispatchers ${DIR_DISP_SRCS})
