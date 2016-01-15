cmake_minimum_required(VERSION 2.8)

project(unabto_linux_test)

set(UNABTO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
include(${UNABTO_ROOT}/build/cmake/unabto_files.cmake)

option(LOG_ALL "Enable all log output" OFF)

if (LOG_ALL)
  add_definitions(-DNABTO_LOG_ALL=1)
endif()

include_directories(
  ${UNABTO_PLATFORMS_SRC_DIR}/unix
  ${unabto_core_include_directories}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${UNABTO_SERVER_TEST_DIR}
)

set(src 
  ${unabto_server_common_src_test}
  ${unabto_unix_src_minimal}
  ${unabto_server_stream_test_src}
  ${unabto_unix_src_minimal}
  ${unabto_network_bsd_src}
  ${unabto_module_unix_dns_src}
  ${unabto_application_adapter_dummy_src}
  ${CMAKE_CURRENT_SOURCE_DIR}/runLinuxTest.c)

add_definitions(-Wall)
link_libraries(${PROJECT_NAME} pthread rt m)

add_executable(${PROJECT_NAME} ${src})

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
