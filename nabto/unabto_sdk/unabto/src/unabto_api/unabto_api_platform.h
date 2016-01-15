/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifndef _UNABTO_API_PLATFORM_
#define _UNABTO_API_PLATFORM_

/**
 * A generic platform interface which should be supported by the underlying platform.
 */

#include "unabto_api.h"
#include <unabto/unabto_packet_util.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * init the platform
 */
unabto_api_status_t unabto_api_platform_init();

/**
 * Start the unabto_platform. Unless otherwise specified this
 * creates a kbackground process which runs the unabto platform.
 */
unabto_api_status_t unabto_api_platform_start();

/**
 * Stop unabto platform.
 */
unabto_api_status_t unabto_api_platform_stop();

/**
 * Set the name of the uNabto platform.
 */
unabto_api_status_t unabto_api_platform_set_name(const char* name);

/**
 * Set the local port uNabto uses for local connections and discovery
 */
unabto_api_status_t unabto_api_platform_set_local_port(int port);

/**
 * Set the encryption method and key
 */
unabto_api_status_t unabto_api_platform_set_crypto_suite(crypto_suite crypto_suit, uint8_t* key, size_t key_length);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
