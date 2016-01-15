/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifndef UNABTO_API_COMMON_H
#define UNABTO_API_COMMON_H

#include "unabto_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Blocks unabto until the mutex are unlocked.
 */
void unabto_api_lock();

/**
 * Unlocks a blocked mutex.
 */
void unabto_api_unlock();

/**
 * Yield the current executing thread.
 */
void unabto_api_yield();

/**
 * @return true if we are blocking
 */
bool is_blocking();


#ifdef __cplusplus
} //extern "C"
#endif

#endif
