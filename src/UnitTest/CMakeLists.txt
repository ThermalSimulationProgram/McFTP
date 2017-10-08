aux_source_directory(. DIR_test_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -g -O3 -std=c++11 -I ../")
add_library(UnitTest STATIC ${DIR_test_SRCS})
