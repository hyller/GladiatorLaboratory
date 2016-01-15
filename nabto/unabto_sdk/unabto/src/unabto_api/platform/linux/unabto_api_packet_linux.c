/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api_packet.h"
#include "unabto_util.h"
#include <modules/mt/unabto_mutex.h>

unabto_mutex_t sendLock;

void packetInit() {
    unabto_mutex_init(&sendLock);
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

    buf = (uint8_t*)malloc(len);
    if (buf == NULL) {
        NABTO_LOG_TRACE(("Receive Event failed to allocate %ul bytes.", len));
        NABTO_LOG_BUFFER(NABTO_LOG_SEVERITY_TRACE, ("Length buffer"),lengthBuffer,sizeof(uint32_t));
        return false;
    }

    if (unabto_api_stream_read_all(stream, buf, len) != UNABTO_API_OK) {
        NABTO_LOG_TRACE(("failed to read event"));
        free(buf);
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
        unabto_mutex_lock(&sendLock);
         
        if (unabto_api_stream_write_all(stream, lengthBuffer, sizeof(uint32_t)) != UNABTO_API_OK) {
            NABTO_LOG_TRACE(("failed to send packet length"));
            ret = false;
        }
        else if (unabto_api_stream_write_all(stream, packet, packetLength) != UNABTO_API_OK) {
            NABTO_LOG_TRACE(("failed to send packet data"));
            ret = false;
        }

        unabto_mutex_unlock(&sendLock);
    }
    return ret;
}
