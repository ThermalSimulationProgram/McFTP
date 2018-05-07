aux_source_directory(. DIR_CMI_SRCS)


set (CMAKE_CXX_FLAGS "-Wall -g -O3 -std=c++11 -I ../")

add_subdirectory(./interfaces)
add_subdirectory(./benchmarks)
add_subdirectory(./UnitTest)
add_subdirectory(./configuration)
add_subdirectory(./core)
add_subdirectory(./dispatchers)
add_subdirectory(./pthread)
add_subdirectory(./results)
add_subdirectory(./tasks)
add_subdirectory(./taskloads)
add_subdirectory(./performance_counter)
add_subdirectory(./soft_temperature_sensor)
add_subdirectory(./utils)



add_library(cmi STATIC  ${DIR_CMI_SRCS} )




target_link_libraries(cmi  UnitTest core interfaces configuration  dispatchers pthread results tasks taskloads utils performance_counter soft_sensor benchmarks)


set_target_properties(cmi PROPERTIES ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/)
