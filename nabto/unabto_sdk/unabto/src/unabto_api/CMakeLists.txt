cmake_minimum_required(VERSION 2.8)
project(unabto_api)

set(UNABTO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
include(${UNABTO_ROOT}/build/cmake/unabto_files.cmake)

include_directories(.)
include_directories(${unabto_core_include_directories})
include_directories(../weather_station)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pc_config)

add_definitions(-Wall)

set(src
  ${CMAKE_CURRENT_SOURCE_DIR}/unabto_api.c
  ${CMAKE_CURRENT_SOURCE_DIR}/unabto_api_common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/platform/linux/unabto_api_linux.c
)

link_libraries(unabto_api pthread)

add_library("${PROJECT_NAME}" SHARED ${unabto_server_unabto_unix_src} ${unabto_server_common_src} ${src})

install( TARGETS ${PROJECT_NAME} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

set(test_src
#  ${CMAKE_CURRENT_SOURCE_DIR}/unabto_api_test.c
#  ${CMAKE_CURRENT_SOURCE_DIR}/../weather_station/unabto_weather_station.c
  ${CMAKE_CURRENT_SOURCE_DIR}/unabto_api_test_application.c
  ${CMAKE_CURRENT_SOURCE_DIR}/unabto_api_test_linux.c
  
)

link_libraries(unabto_api_test pthread)


add_executable(unabto_api_test ${test_src} ${unabto_server_unabto_unix_src} ${unabto_server_common_src} ${src})
