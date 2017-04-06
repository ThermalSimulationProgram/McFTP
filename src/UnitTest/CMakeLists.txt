aux_source_directory(. DIR_test_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -std=c++11 -I ../")
add_library(UnitTest ${DIR_test_SRCS})
