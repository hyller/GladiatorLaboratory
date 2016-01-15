#ifndef _TCP_FALLBACK_LINUX_H_
#define _TCP_FALLBACK_LINUX_H_

#include <modules/mt/unabto_thread.h>
#include <modules/mt/unabto_mutex.h>
#include <modules/mt/unabto_cond.h>
#include <unabto_platform_types.h>

typedef struct unabto_tcp_fallback_connection {
    unabto_thread_t sendThread;
    unabto_thread_t receiveThread;
    
    unabto_mutex_t sendBufferMutex;
    unabto_cond_t  sendBufferCondition;
    uint8_t        sendBuffer[65536];
    size_t         sendBufferLength;

    unabto_mutex_t recvRunningMutex;
    unabto_mutex_t sendRunningMutex;
    unabto_cond_t  recvRunningCondition;
    unabto_cond_t  sendRunningCondition;
    bool           recvRunning;
    bool           sendRunning;

    volatile bool closed;
    int           socket;
} unabto_tcp_fallback_connection;


#endif
