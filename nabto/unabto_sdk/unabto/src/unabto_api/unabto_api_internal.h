/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifndef _UNABTO_API_INTERNAL_H_
#define _UNABTO_API_INTERNAL_H_

#include <unabto_api/unabto_api.h>
#include <modules/mt/unabto_mutex.h>

/**
 * if an unabto api implements these functions it's possible to use
 * the api with not yet api exposed unabto functions.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * take/release a lock of the internal datastructures of all unabto
 */
unabto_api_status_t unabto_api_lock_take();
unabto_api_status_t unabto_api_lock_release();
unabto_api_status_t unabto_api_yield_notify();
unabto_mutex_t* unabto_api_get_mutex();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
