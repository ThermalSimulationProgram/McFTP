aux_source_directory(. DIR_PERFCOUNTER_SRCS)
set (CMAKE_CXX_FLAGS "-Wall  -g -O3 -std=c++11 ")


if(SOFT_TEMPERATURE_SENSOR_ENABLE  STREQUAL "ON")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../third_party_lib/papi-5.5.1/src/testlib/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../third_party_lib/papi-5.5.1/src/)

endif(SOFT_TEMPERATURE_SENSOR_ENABLE  STREQUAL "ON")



add_library(performance_counter STATIC ${DIR_PERFCOUNTER_SRCS})

set_target_properties(performance_counter PROPERTIES LINK_FLAGS "-Wl,--whole-archive")

if(SOFT_TEMPERATURE_SENSOR_ENABLE  STREQUAL "ON")
target_link_libraries(performance_counter ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party_lib/papi-5.5.1/src/testlib/libtestlib.a ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party_lib/papi-5.5.1/src/libpapi.a)
endif(SOFT_TEMPERATURE_SENSOR_ENABLE  STREQUAL "ON")
