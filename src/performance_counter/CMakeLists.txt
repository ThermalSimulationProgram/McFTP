aux_source_directory(. DIR_BENCH_SRCS)
set (CMAKE_CXX_FLAGS "-Wall  -g -O3 -std=c++11 ")



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/papi-5.5.1/src/testlib/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/papi-5.5.1/src/)



add_library(performance_counter ${DIR_BENCH_SRCS})

target_link_libraries(performance_counter ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/papi-5.5.1/src/testlib/libtestlib.a ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/papi-5.5.1/src/libpapi.a)
