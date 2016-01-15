/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifndef UNABTO_API_PACKET_H
#define UNABTO_API_PACKET_H

#include "unabto_api.h"

#include "FreeRTOS.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

void packetInit();

bool readPacket(unabto_api_stream_t stream, uint8_t **packet, size_t* packetLength);

bool sendPacket(unabto_api_stream_t stream, uint8_t* packet, size_t packetLength);

void receiver_task(void *params);
void sender_task(void *params);

typedef struct {
    void* buffer;
    size_t length;
} packet_queue_item_t;

typedef struct {
    unabto_api_stream_t stream;
    xQueueHandle queue;
} packet_task_state_t;


#ifdef __cplusplus
} //extern "C"
#endif

#endif
