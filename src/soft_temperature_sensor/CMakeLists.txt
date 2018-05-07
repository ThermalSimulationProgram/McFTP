aux_source_directory(. DIR_SOFTSENSOR_SRCS)
set (CMAKE_CXX_FLAGS "-Wall  -g -O3 -std=c++11 -I ../")
SET ( CMAKE_EXE_LINKER_FLAGS  "-lm" )
add_library(soft_sensor STATIC ${DIR_SOFTSENSOR_SRCS})
