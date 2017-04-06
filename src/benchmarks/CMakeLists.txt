aux_source_directory(. DIR_BENCH_SRCS)
set (CMAKE_CXX_FLAGS "-Wall -std=c++11 -I ../")
add_library(benchmarks ${DIR_BENCH_SRCS})
