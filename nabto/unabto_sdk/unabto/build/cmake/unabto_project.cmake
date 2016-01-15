# This project makes a default project for unabto.
# It finds the default platform.
# A default random module, can be overriden by UNABTO_RANDOM_MODULE
# A default crypto module, can be overridden by UNABTO_CRYPTO_MODULE
# A default network module, can be overridden by UNABTO_NETWORK_MODULE


# UNABTO_RANDOM_MODULE
# possibilities: dummy openssl libtomcrypt none

# UNABTO_CRYPTO_MODULE
# possibilities: generic openssl libtomcrypt none

# UNABTO_NETWORK_MODULE
# possibilities: bsd winsock none

# An external library dir can be added such that e.g. an openssl library can be found there
# Specify the cmake option UNABTO_EXTERNAL_BUILD_DIR to give a hint about the library location.

set(UNABTO_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)

include(${UNABTO_ROOT}/build/cmake/unabto_files.cmake)
include (CheckIncludeFiles)
include (CheckFunctionExists)
INCLUDE (CheckLibraryExists)

set(unabto_src
  ${unabto_core_src}
)

set(unabto_include_directories
  ${UNABTO_INCLUDE_DIR}
)

set(unabto_definitions

)


set(unabto_link_libraries

)

# Detect crypto suite
CHECK_INCLUDE_FILES (openssl/rand.h HAVE_OPENSSL_RANDOM_H)
CHECK_INCLUDE_FILES (openssl/evp.h HAVE_OPENSSL_EVP_H)
CHECK_INCLUDE_FILES (openssl/hmac.h HAVE_OPENSSL_HMAC_H)

if (UNABTO_EXTERNAL_BUILD_ROOT)
  if (EXISTS ${UNABTO_EXTERNAL_BUILD_ROOT}/lib/libcrypto.a)
    SET(HAVE_OPENSSL_EXTERNAL_CRYPTO_LIB 1)
  endif()
endif()

if (NOT UNABTO_RANDOM_MODULE)
  if (HAVE_OPENSSL_RANDOM_H OR HAVE_OPENSSL_EXTERNAL_CRYPTO_LIB)
    set(UNABTO_RANDOM_MODULE openssl)
  else()
    set(UNABTO_RANDOM_MODULE libtomcrypt)
  endif()
endif()

if (NOT UNABTO_CRYPTO_MODULE)
  if ((HAVE_OPENSSL_EVP_H AND HAVE_OPENSSL_HMAC_H) OR HAVE_OPENSSL_EXTERNAL_CRYPTO_LIB)
    set(UNABTO_CRYPTO_MODULE openssl)
  else()
    set(UNABTO_CRYPTO_MODULE libtomcrypt)
  endif()
endif()

if (NOT UNABTO_NETWORK_MODULE)
  if (WIN32)
    set(UNABTO_NETWORK_MODULE winsock)
  else()
    set(UNABTO_NETWORK_MODULE bsd)
  endif()
endif()

if (UNABTO_RANDOM_MODULE MATCHES dummy)
  list(APPEND unabto_src ${unabto_module_random_dummy})
elseif(UNABTO_RANDOM_MODULE MATCHES openssl)
  list(APPEND unabto_src ${unabto_module_openssl_random})
  if (HAVE_OPENSSL_EXTERNAL_CRYPTO_LIB)
    list(APPEND unabto_link_libraries ${UNABTO_EXTERNAL_BUILD_ROOT}/lib/libcrypto.a)
  else()
    list(APPEND unabto_link_libraries crypto)
  endif()
  list(APPEND unabto_link_libraries dl)
elseif(UNABTO_RANDOM_MODULE MATCHES libtomcrypt)
  list(APPEND unabto_include_directories ${unabto_random_ltc_include_directories})
  list(APPEND unabto_src ${unabto_random_ltc_src})
else()
  message(WARNING "No random module")
endif()

if (UNABTO_CRYPTO_MODULE MATCHES generic)
  list(APPEND unabto_src ${unabto_crypto_generic_src})
elseif(UNABTO_CRYPTO_MODULE MATCHES libtomcrypt)
  list(APPEND unabto_include_directories ${unabto_crypto_ltc_include_directories})
  list(APPEND unabto_src ${unabto_crypto_ltc_src})
  list(APPEND unabto_definitions -DLTC_NO_ASM)
  if (WIN32)
    list(APPEND unabto_definitions "-DLTC_EXPORT=extern" )
  endif()
  if (CMAKE_SYSTEM_NAME MATCHES "WindowsCE")
    list(APPEND unabto_link_libraries Coredll)
    list(APPEND unabto_definitions "-DARGTYPE=2")
  endif()
elseif(UNABTO_CRYPTO_MODULE MATCHES openssl)
  list(APPEND unabto_src ${unabto_crypto_openssl_src})
  if (HAVE_OPENSSL_EXTERNAL_CRYPTO_LIB)
    list(APPEND unabto_link_libraries ${UNABTO_EXTERNAL_BUILD_ROOT}/lib/libcrypto.a)
  else()
    list(APPEND unabto_link_libraries crypto)
  endif()
  list(APPEND unabto_link_libraries dl)
else()
  message(WARNING "No crypto module")
endif()

if (UNABTO_NETWORK_MODULE MATCHES bsd)
  list(APPEND unabto_src ${unabto_network_bsd_src})
elseif(UNABTO_NETWORK_MODULE MATCHES winsock)
  list(APPEND unabto_src ${unabto_module_winsock_src})
endif()

if (UNABTO_EXTERNAL_BUILD_ROOT)
  list(APPEND unabto_include_directories ${UNABTO_EXTERNAL_BUILD_ROOT}/include)
endif()

if(${CMAKE_SYSTEM} MATCHES Linux)

  # Glibc 2.17 and newer dows not use -lrt and moxa does not have the lib at all.
  CHECK_LIBRARY_EXISTS(rt clock_gettime "" HAVE_RT_LIB)
  CHECK_LIBRARY_EXISTS(pthread pthread_create "" HAVE_PHTREAD_LIB)

  list(APPEND unabto_include_directories
    ${UNABTO_PLATFORMS_SRC_DIR}/unix
    )

  list(APPEND unabto_src
    ${unabto_module_unix_dns_src}
    ${UNABTO_PLATFORMS_SRC_DIR}/unix/unabto_environment_unix.c
    ${UNABTO_PLATFORMS_SRC_DIR}/unabto_printf_logger.c
    ${UNABTO_MODULES_SRC_DIR}/log/unabto_basename.c
    ${UNABTO_MODULES_SRC_DIR}/log/unix/unabto_log_header_unix.c
    ${UNABTO_MODULES_SRC_DIR}/timers/unix/unabto_unix_time.c
    )
  list(APPEND unabto_link_libraries m)

  if (HAVE_PHTREAD_LIB)
    list(APPEND unabto_link_libraries pthread)
  else()
    list(APPEND unabto_definitions -pthread)
  endif()

  if (MOXA)
  elseif (HAVE_RT_LIB)
    list(APPEND unabto_link_libraries rt)
  endif()

endif()

if (WIN32)
  list(APPEND unabto_include_directories
    ${UNABTO_PLATFORMS_SRC_DIR}/win32
    )
  list(APPEND unabto_src
    ${unabto_server_common_src}
    ${unabto_gopt_src}
    ${UNABTO_PLATFORMS_SRC_DIR}/win32/unabto_platform.c
    ${UNABTO_MODULES_SRC_DIR}/log/unabto_basename.c
    ${UNABTO_MODULES_SRC_DIR}/log/win32/unabto_log_header_win32.c
    ${UNABTO_PLATFORMS_SRC_DIR}/
    ${UNABTO_PLATFORMS_SRC_DIR}/unabto_printf_logger.c
    ${unabto_module_winsock_dns_src}
    )
  if (CMAKE_SYSTEM_NAME MATCHES "WindowsCE")
    list(APPEND unabto_link_libraries Ws2)
  else()
    list(APPEND unabto_link_libraries Ws2_32)
  endif()
  list(APPEND unabto_definitions -DWIN32_LEAN_AND_MEAN)
endif()

if (APPLE)
  list(APPEND unabto_include_directories
    ${UNABTO_PLATFORMS_SRC_DIR}/unix
    )

  list(APPEND unabto_src
    ${unabto_module_unix_dns_src}
    ${UNABTO_PLATFORMS_SRC_DIR}/unix/unabto_environment_unix.c
    ${UNABTO_PLATFORMS_SRC_DIR}/unabto_printf_logger.c
    ${UNABTO_MODULES_SRC_DIR}/log/unabto_basename.c
    ${UNABTO_MODULES_SRC_DIR}/log/unix/unabto_log_header_unix.c
    ${UNABTO_MODULES_SRC_DIR}/timers/unix/unabto_unix_time.c
    )
  list(APPEND unabto_link_libraries pthread m)
endif()


message(STATUS "Configuration of uNabto")
message(STATUS "  Crypto module: " ${UNABTO_CRYPTO_MODULE})
message(STATUS "  Random module: " ${UNABTO_RANDOM_MODULE})
message(STATUS "  Network module: " ${UNABTO_NETWORK_MODULE})
