cmake_minimum_required(VERSION 2.8)
project(unabto-tunnel)

set(UNABTO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
include(${UNABTO_ROOT}/build/cmake/unabto_project.cmake)
include(${UNABTO_ROOT}/build/cmake/unabto_version.cmake)

option(USE_LIB_PROFILER "Link executables with libprofiler.so" OFF)
option(LOG_ALL "Print all unabto log statements" OFF)
option(USE_TEST_WEBSERVER "Enable a mongoose test webserver" OFF)

add_definitions(-DUNABTO_NETWORK_BSD_NONBLOCKING=0)

if(EXTERNAL_PREFIX) 
  include_directories(${EXTERNAL_PREFIX}/include)
  link_directories(${EXTERNAL_PREFIX}/lib)
endif()

set(unabto_server_unabto_tunneling_src ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if (LOG_ALL)
  add_definitions(-DLOG_ALL)
endif()

list(APPEND unabto_src
  ${unabto_server_unabto_tunneling_src}/tunnel.c
  ${CMAKE_CURRENT_SOURCE_DIR}/unabto_config.h
  ${UNABTO_PLATFORMS_SRC_DIR}/unabto_printf_logger.c
  ${unabto_gopt_src}
  ${UNABTO_MODULES_SRC_DIR}/log/unabto_basename.c
  ${unabto_module_diagnostics_src}
  ${unabto_module_dynamic_log_src}
)


list(APPEND unabto_src
  ${unabto_module_syslog_src}
)

if (USE_TEST_WEBSERVER)
  list(APPEND unabto_src
	${unabto_server_unabto_tunneling_src}/mongoose/mongoose.c
	${unabto_server_unabto_tunneling_src}/test_webserver.c
	)
  
  add_definitions(-DUSE_TEST_WEBSERVER=1)
endif()

SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc")

list(APPEND unabto_src  ${UNABTO_ROOT}/src/modules/tcp_fallback/tcp_fallback_select.c)
add_definitions(-DNABTO_ENABLE_TCP_FALLBACK=1)

include_directories(
  # make it possible to override unabto_config.h with a supplied file
  # to the build tree.
  ${CMAKE_CURRENT_BINARY_DIR}

  ${unabto_include_directories}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  )

add_definitions(${unabto_definitions})

if (USE_LIB_PROFILER)
  list(APPEND unabto_link_libraries profiler)
endif()

add_executable(test_tunnels ${unabto_src} ${CMAKE_CURRENT_SOURCE_DIR}/src/test_tunnels.c)

target_link_libraries(test_tunnels ${unabto_link_libraries})


install( TARGETS test_tunnels
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
