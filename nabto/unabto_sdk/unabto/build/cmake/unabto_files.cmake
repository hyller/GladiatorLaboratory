# CMake scipt: Define Nabto directory locations and lists of files.
# set(UNABTO_ROOT ...) to location of unabto directory before including this cmake script.

# Define different include directories to be included in a project file
set(UNABTO_INCLUDE_DIR      ${UNABTO_ROOT}/src)
set(UNABTO_INCLUDE_API_DIR  ${UNABTO_ROOT}/src/unabto_api)

# Define different root directories containing source code to be included in a project file
set(UNABTO_MODULES_SRC_DIR   ${UNABTO_ROOT}/src/modules)
set(UNABTO_PLATFORMS_SRC_DIR ${UNABTO_ROOT}/src/platforms)
set(UNABTO_API_SRC_DIR       ${UNABTO_ROOT}/src/unabto_api)
set(UNABTO_SERVER_SRC_DIR    ${UNABTO_ROOT}/src)

# Options to change compilation
option(USE_OPENSSL "Use openssl instead of the generic encryption code" OFF)
option(USE_DUMMY_RANDOM "Use dummy random" OFF)

# Other useful directories
set(UNABTO_SERVER_TEST_DIR ${UNABTO_ROOT}/test)
set(UNABTO_DEMO_DIR ${UNABTO_ROOT}/demo)
set(UNABTO_DEMO_COMMON_DIR ${UNABTO_DEMO_DIR}/common)

set(unabto_module_random_dummy
  ${UNABTO_MODULES_SRC_DIR}/random/dummy/unabto_random_dummy.c
)

set(unabto_module_openssl_random
  ${UNABTO_MODULES_SRC_DIR}/random/openssl/unabto_random_openssl.c
)

set(unabto_module_timers_boost
  ${UNABTO_MODULES_SRC_DIR}/timers/boost/unabto_time_boost.cpp
)

set(unabto_module_winsock_src
  ${UNABTO_MODULES_SRC_DIR}/network/winsock/unabto_winsock.c
)

set (unabto_module_winsock_dns_src
  ${UNABTO_MODULES_SRC_DIR}/network/winsock/unabto_winsock_dns.c
)

set(unabto_module_unix_dns_src
  ${UNABTO_MODULES_SRC_DIR}/dns/unix/unabto_unix_dns.c
)

set(unabto_module_win32_dns_src
  ${UNABTO_MODULES_SRC_DIR}/dns/win32/unabto_win32_dns.c
)

set(unabto_module_syslog_src
  ${UNABTO_MODULES_SRC_DIR}/log/syslog/unabto_syslog.c
)

set(unabto_module_dynamic_log_src
  ${UNABTO_MODULES_SRC_DIR}/log/dynamic/unabto_dynamic_log.c
  ${UNABTO_MODULES_SRC_DIR}/log/dynamic/unabto_dynamic_log_util.c
)

set(unabto_module_diagnostics_src
  ${UNABTO_MODULES_SRC_DIR}/diagnostics/unabto_diag.c
)


set(unabto_server_common_src_test
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_buffers.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_query_rw.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/util/unabto_buffer.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/util/unabto_queue.c
  ${UNABTO_SERVER_TEST_DIR}/modules/crypto/generic/unabto_sha256_test.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/unabto_hmac_sha256_test.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/unabto_test.c
  ${UNABTO_SERVER_TEST_DIR}/modules/crypto/generic/unabto_aes_test.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/unabto_aes_cbc_test.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/unabto_prfplus_test.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/unabto_aes128_sha256_test.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/util/unabto_util_test.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/util/unabto_buffer_test.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/unabto_crypto_test.c
  ${UNABTO_SERVER_TEST_DIR}/modules/util/unabto_base32_test.c
  ${UNABTO_SERVER_SRC_DIR}/modules/util/unabto_base32.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_crypto.c
  ${unabto_module_random_dummy}
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_prf.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_aes128_sha256.c
)


set(unabto_crypto_ltc_src
  ${UNABTO_MODULES_SRC_DIR}/crypto/libtomcrypt/unabto_libtomcrypt.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/ciphers/aes/aes.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/modes/cbc/cbc_decrypt.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/modes/cbc/cbc_done.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/modes/cbc/cbc_encrypt.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/modes/cbc/cbc_start.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/hashes/sha2/sha256.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/hashes/helper/hash_memory.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/mac/hmac/hmac_init.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/mac/hmac/hmac_process.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/mac/hmac/hmac_done.c

  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_argchk.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_cipher_is_valid.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_cipher_descriptor.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_find_cipher.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_register_cipher.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_register_hash.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_find_hash.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_hash_is_valid.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_hash_descriptor.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/zeromem.c
)

set(unabto_crypto_ltc_include_directories
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/headers
)

set(unabto_random_ltc_src 
  ${UNABTO_MODULES_SRC_DIR}/random/ltc/unabto_random_ltc.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/prngs/rng_get_bytes.c
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/misc/crypt/crypt_argchk.c
)

set(unabto_random_ltc_include_directories
  ${UNABTO_ROOT}/3rdparty/libtomcrypt/src/headers
)

set(unabto_crypto_generic_src
  ${UNABTO_MODULES_SRC_DIR}/crypto/generic/unabto_aes.c
  ${UNABTO_MODULES_SRC_DIR}/crypto/generic/unabto_aes_cbc.c
  ${UNABTO_MODULES_SRC_DIR}/crypto/generic/unabto_hmac_sha256.c
  ${UNABTO_MODULES_SRC_DIR}/crypto/generic/unabto_sha256.c
)

  
set(unabto_crypto_openssl_src
  ${UNABTO_MODULES_SRC_DIR}/crypto/openssl/unabto_crypto_openssl.c
)


set(unabto_crypto_src
)

if (USE_LIBTOMCRYPT)
  list(APPEND unabto_crypto_src ${unabto_crypto_ltc_src})
elseif(USE_OPENSSL)
  list(APPEND unabto_crypto_src ${unabto_crypto_openssl_src})
else()
  list(APPEND unabto_crypto_src ${unabto_crypto_generic_src})
endif()


set(unabto_core_src 
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_app_adapter.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_attach.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/util/unabto_buffer.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_connection.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_context.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_crypto.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_message.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_packet.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_packet_util.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/util/unabto_queue.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_stream.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_stream_event.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_stream_window.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_stream_environment.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_nano_stream.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_common_main.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_next_event.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_prf.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_aes128_sha256.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_buffers.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_query_rw.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_tcp_fallback.c
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_debug_packet.c
)

set(unabto_server_common_src 
  ${unabto_core_src}
)

set(unabto_server_all_common_src 
  ${unabto_server_common_src}
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_tcp_fallback.c
)

set(unabto_unix_src_minimal
  ${UNABTO_PLATFORMS_SRC_DIR}/unix/unabto_environment_unix.c
  ${UNABTO_PLATFORMS_SRC_DIR}/unabto_printf_logger.c
  ${UNABTO_MODULES_SRC_DIR}/log/unabto_basename.c
  ${UNABTO_MODULES_SRC_DIR}/log/unix/unabto_log_header_unix.c
  ${UNABTO_MODULES_SRC_DIR}/timers/unix/unabto_unix_time.c
  ${unabto_crypto_src}
)

set(unabto_pc_src_minimal # for use with unit tests
  ${UNABTO_PLATFORMS_SRC_DIR}/win32/unabto_platform.c
  ${UNABTO_PLATFORMS_SRC_DIR}/unabto_printf_logger.c
  ${UNABTO_MODULES_SRC_DIR}/log/win32/unabto_log_header_win32.c
  ${unabto_crypto_src}
)

set(unabto_gopt_src
  ${UNABTO_MODULES_SRC_DIR}/cli/gopt/gopt.c
  ${UNABTO_MODULES_SRC_DIR}/cli/gopt/unabto_args_gopt.c
  ${UNABTO_MODULES_SRC_DIR}/diagnostics/unabto_diag.c
)


set(unabto_server_unabto_pc_src 
  ${unabto_server_common_src}
  ${unabto_gopt_src}
  ${unabto_pc_src_minimal}
  ${unabto_module_random_dummy}
  ${unabto_module_winsock_src}
  ${unabto_module_winsock_dns_src}
)

set(unabto_network_bsd_src
  ${UNABTO_MODULES_SRC_DIR}/network/bsd/unabto_network_bsd.c
)

set(unabto_application_adapter_dummy_src
  ${UNABTO_MODULES_SRC_DIR}/application_event/dummy/application_event_dummy.c
)

set(unabto_server_unix_src 
  ${unabto_gopt_src}
  ${unabto_unix_src_minimal}
  ${unabto_network_bsd_src}
  ${unabto_module_unix_dns_src}
)

set(unabto_server_win32_src 
  ${unabto_gopt_src}
  ${unabto_pc_src_minimal}
  ${unabto_module_winsock_src}
  ${unabto_module_win32_dns_src}
)

if(USE_DUMMY_RANDOM)
  list(APPEND unabto_server_unix_src ${unabto_module_random_dummy})
  list(APPEND unabto_server_win32_src ${unabto_module_random_dummy})
else()
  list(APPEND unabto_server_unix_src ${unabto_module_openssl_random})
  list(APPEND unabto_server_win32_src ${unabto_module_random_dummy})
endif()

set(unabto_server_unabto_unix_src ${unabto_server_unix_src}) # Deprecated

set(unabto_server_microchip_src
  ${UNABTO_MODULES_SRC_DIR}/network/microchip/unabto_microchip_udp.c
  #${UNABTO_PLATFORMS_SRC_DIR}/microchip/unabto_environment_mchip.c
  #${UNABTO_PLATFORMS_SRC_DIR}/microchip/unabto_time_mchip.c
)


# unabto_pic32_microchip_files
set(unabto_server_pic32_freertos_src 
  ${UNABTO_PLATFORMS_SRC_DIR}/pic32_freertos/basic_io.c
  ${UNABTO_PLATFORMS_SRC_DIR}/pic32_freertos/unabto_environment_uip.c
  ${UNABTO_PLATFORMS_SRC_DIR}/pic32_freertos/unabto_time_freertos.c
)

# unabto microchip udp files
set(unabto_server_microchip_udp_implementation 
  ${UNABTO_MODULES_SRC_DIR}/network/microchip/unabto_microchip_udp.c
  #${UNABTO_PLATFORMS_SRC_DIR}/microchip/unabto_environment_mchip.c
)

set(unabto_tunnel_src
  ${UNABTO_DEMO_COMMON_DIR}/unabto_tunnel.c
)

set(unabto_command_src
  ${UNABTO_DEMO_COMMON_DIR}/unabto_command.c
)

set(unabto_server_weather_station
  ${UNABTO_DEMO_DIR}/weather_station/unabto_main.c
  ${UNABTO_DEMO_DIR}/weather_station/unabto_weather_station.c
)

set(unabto_weather_station_test
  ${UNABTO_DEMO_DIR}/weather_station/unabto_weather_station_test.c
)


set(unabto_api_src
  ${UNABTO_API_SRC_DIR}/unabto_api.c
  ${UNABTO_API_SRC_DIR}/unabto_api_common_internal.c
)

set(unabto_api_linux_src
  ${unabto_server_all_common_src}
  ${unabto_server_unix_src}
  ${unabto_api_src}
  ${UNABTO_API_SRC_DIR}/unabto_api_mt.c
  ${UNABTO_API_SRC_DIR}/platform/linux/unabto_api_linux.c
)

set(unabto_api_win32_src
  ${unabto_server_all_common_src}
  ${unabto_server_win32_src}
  ${unabto_api_src}
  ${UNABTO_API_SRC_DIR}/unabto_api_mt.c
  ${UNABTO_API_SRC_DIR}/platform/linux/unabto_api_linux.c
)

set(unabto_core_include_directories
  ${UNABTO_INCLUDE_DIR}
)

set(unabto_api_linux_includes
  ${unabto_core_include_directories}
  ${UNABTO_INCLUDE_API_DIR}
)

set(unabto_api_win32_includes
  ${UNABTO_INCLUDE_API_DIR}
  ${UNABTO_INCLUDE_DIR}
)

set(unabto_api_freertos_src
  ${unabto_server_common_src}
  ${unabto_api_src}
  ${UNABTO_API_SRC_DIR}/unabto_api_common.c
  ${UNABTO_API_SRC_DIR}/platform/freertos/unabto_api_freertos.c
)

set(unabto_tick_timer_src
  ${UNABTO_MODULES_SRC_DIR}/timers/tick_timer/unabto_tick_timer.c
)

set(unabto_module_varargs_to_cstring_src
  ${UNABTO_MODULES_SRC_DIR}/log/unabto_varargs_to_cstring.c
)

set(unabto_server_stream_test_src
  ${UNABTO_SERVER_SRC_DIR}/unabto/unabto_stream_event.c
  ${UNABTO_SERVER_TEST_DIR}/unabto/unabto_stream_event_test.c
  ${unabto_core_src}
)

set (unabto_module_coap_src
  ${UNABTO_MODULES_SRC_DIR}/coap/er-coap-13/er-coap-13-engine.c
  ${UNABTO_MODULES_SRC_DIR}/coap/er-coap-13/er-coap-13-observing.c
  ${UNABTO_MODULES_SRC_DIR}/coap/er-coap-13/er-coap-13-separate.c
  ${UNABTO_MODULES_SRC_DIR}/coap/er-coap-13/er-coap-13-transactions.c
  ${UNABTO_MODULES_SRC_DIR}/coap/er-coap-13/er-coap-13.c
  ${UNABTO_MODULES_SRC_DIR}/coap/etimer.c
  ${UNABTO_MODULES_SRC_DIR}/coap/unabto_uip.c
  ${UNABTO_MODULES_SRC_DIR}/coap/list.c
  ${UNABTO_MODULES_SRC_DIR}/coap/erbium/erbium.c
  ${UNABTO_MODULES_SRC_DIR}/coap/er_coap_nabto.c
)

set (unabto_module_coap_includes
  ${UNABTO_MODULES_SRC_DIR}/coap
  ${UNABTO_MODULES_SRC_DIR}/coap/er-coap-13
  ${UNABTO_MODULES_SRC_DIR}/coap/erbium
)
