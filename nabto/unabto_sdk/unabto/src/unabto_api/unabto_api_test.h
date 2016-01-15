/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifndef UNABTO_API_TEST_H
#define UNABTO_API_TEST_H

#include "unabto_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A thread which process uNabto ticks.
 */
void *unabto_thread(void *params);

/**
 * A thread when started echos all data read on the stream
 * back on the stream.
 */
void *echo(void* params);


#ifdef __cplusplus
} //extern "C"
#endif

#endif
