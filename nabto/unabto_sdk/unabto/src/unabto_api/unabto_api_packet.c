/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#define NABTO_DECLARED_MODULE NABTO_LOG_UNABTO_API

#include "unabto_api_packet.h"
#include "unabto/unabto_util.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

xSemaphoreHandle sendLock;

void packetInit() {
    sendLock = xSemaphoreCreateMutex();
}

bool readPacket(unabto_api_stream_t stream, uint8_t** packet, size_t* packetLength) {
    uint8_t lengthBuffer[sizeof(uint32_t)];
    uint32_t len;
    uint8_t* buf;
    if (unabto_api_stream_read_all(stream, lengthBuffer, sizeof(uint32_t)) != UNABTO_API_OK) {
        NABTO_LOG_TRACE(("failed to read length of event"));
        return false;
    }
    
    unabto_api_lock();
    READ_U32(len, lengthBuffer);
    unabto_api_unlock();
    
    //if (len > 100) {
        NABTO_LOG_TRACE(("packet length: %i", len));
        //}

    buf = (uint8_t*)pvPortMalloc(len);
    if (buf == NULL) {
        NABTO_LOG_TRACE(("Receive Event failed to allocate %ul bytes.", len));
        NABTO_LOG_BUFFER(NABTO_LOG_SEVERITY_TRACE, ("Length buffer"),lengthBuffer,sizeof(uint32_t));
        return false;
    }

    if (unabto_api_stream_read_all(stream, buf, len) != UNABTO_API_OK) {
        NABTO_LOG_TRACE(("failed to read event"));
        vPortFree(buf);
        return false;
    }

    *packetLength = len;
    *packet = buf;
    return true;
}

bool sendPacket(unabto_api_stream_t stream, uint8_t* packet, size_t packetLength) {
    bool ret = true;
    uint8_t lengthBuffer[sizeof(uint32_t)];
    
    WRITE_U32(lengthBuffer, packetLength);
    {
        xSemaphoreTake(sendLock, portMAX_DELAY);
         
        if (unabto_api_stream_write_all(stream, lengthBuffer, sizeof(uint32_t)) != UNABTO_API_OK) {
            NABTO_LOG_TRACE(("failed to send packet length"));
            ret = false;
        }
        else if (unabto_api_stream_write_all(stream, packet, packetLength) != UNABTO_API_OK) {
            NABTO_LOG_TRACE(("failed to send packet data"));
            ret = false;
        }

        xSemaphoreGive(sendLock);
    }
    return ret;
}

/* void receiver_task(void *params) { */
/*     packet_task_state_t* sts = (packet_task_state_t*)params; */
/*     uint8_t* packet; */
/*     size_t packetLength; */
/*     packet_queue_item_t item; */
/*     for (;;) { */
/*         if (!readPacket(sts->stream, &packet, &packetLength)) { */
/*             NABTO_LOG_TRACE(("failed to receive event")); */
/*             break; */
/*         } else { */
/*             item.buffer = packet; */
/*             item.length = packetLength; */
/*             xQueueSend(sts->queue, &item, 1000/portTICK_RATE_MS); */
/*         } */
/*     } */
/*     vTaskDelete(NULL); */
/* } */

/* void sender_task(void* params) { */
/*     packet_task_state_t* sts = (packet_task_state_t*)params; */
/*     packet_queue_item_t item; */
/*     bool stop = false; */
/*     for (;;) { */
/*         xQueueReceive(sts->queue, &item, portMAX_DELAY); */
/*         if(!sendPacket(sts->stream, item.buffer, item.length)) { */
/*             NABTO_LOG_TRACE(("failed to send item")); */
/*             stop = true; */
/*         } */
/*         vPortFree(item.buffer); */
/*         if (stop) break; */
/*     } */
/*     vTaskDelete(NULL); */
/* } */

