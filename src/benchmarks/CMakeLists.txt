aux_source_directory(. DIR_BENCH_SRCS)
set (CMAKE_CXX_FLAGS "-Wall  -g -O3 -std=c++11 -I ../")
add_library(benchmarks STATIC ${DIR_BENCH_SRCS})
