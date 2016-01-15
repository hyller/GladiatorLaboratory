/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api.h"

#include "unabto_api_common.h"
#include <unabto/unabto_common_main.h>
#include <unabto/unabto_attach.h>
#include <unabto/unabto_stream.h>
#include <unabto/unabto_stream_event.h>
#include <unabto_api/unabto_api_internal.h>
#include <unabto/unabto_external_environment.h>
#include <unabto/unabto_util.h>

#ifdef WIN32
#include <winsock2.h>
#define strdup _strdup
#else
#include <pthread.h>
#include <sched.h>
#include <poll.h>
#include <sys/select.h>
#endif

#include <modules/mt/unabto_thread.h>
#include <modules/mt/unabto_mutex.h>
#include <modules/mt/unabto_cond.h>

#include <time.h>


static DECLARE_UNABTO_MUTEX(lock);

static intptr_t tick_thread_func(void* args);

static unabto_thread_t tick_thread;

static bool running = false;

static nabto_main_setup* setup;

unabto_api_status_t unabto_api_platform_init() {
    setup = unabto_init_context();
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_platform_set_name(const char* n)  {
    setup->id = strdup(n);
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_platform_set_local_port(const int port) {
    setup->localPort = port;
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_platform_set_crypto_suite(crypto_suite suite, uint8_t* key, size_t key_length) {
    setup->secureAttach = true;
    setup->secureData = true;
    setup->cryptoSuite = suite;
    memcpy(setup->presharedKey, key, MIN(PRE_SHARED_KEY_SIZE, key_length));
    return UNABTO_API_OK;
}

/**
 * create a thread which runs the unabto instance.
 */
unabto_api_status_t unabto_api_platform_start() {
    if (running) {
        return UNABTO_API_FAIL;
    }

    if (!unabto_init()) {
        return UNABTO_API_FAIL;
    }

    running = true;
    if (unabto_thread_create(&tick_thread, tick_thread_func, NULL) != 0) {
        NABTO_LOG_FATAL(("Failed to create tick_thread"));
    }

    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_platform_stop() {
    if (running) {
        running = false;
        unabto_thread_join(tick_thread, NULL, NULL);
    }
    unabto_close();
    return UNABTO_API_OK;
}


//void wait_event() {
//}

intptr_t tick_thread_func(void* args) {
    nabto_stamp_t ne;
    nabto_stamp_t shortTime;
    bool received = false;
        
    unabto_mutex_lock(&lock);
    unabto_next_event(&ne);
    unabto_mutex_unlock(&lock);

    nabtoSetFutureStamp(&shortTime, 10);

    while (running) {
        //struct epoll_event events[1];
        int timeout;
        nabto_stamp_t now;
        
        // updating next event is an expensive operation so only do it if 
        // the earlier update is expired.
        if (nabtoIsStampPassed(&ne) || (received && nabtoIsStampPassed(&shortTime))) {
            unabto_mutex_lock(&lock);
            unabto_time_event();
            unabto_next_event(&ne);
            unabto_mutex_unlock(&lock);
            nabtoSetFutureStamp(&shortTime, 10);
            received = false;
        }

        // the problem is that the timing is changed when packets is received.
        

        now = nabtoGetStamp();
        timeout = nabtoStampDiff2ms(nabtoStampDiff(&ne, &now));
        
        if (timeout < 0) {
            NABTO_LOG_DEBUG(("connection timestamp is in the past. setting timeout to 1ms, timeout=%i, this could be a problem.", timeout));
            timeout = 1;
        } else if (timeout == 0) {
            timeout = 1;
        }

        if (received) {
            if (timeout > 10) {
                timeout = 10;
            }
        } else {
            if (timeout > 100) {
                timeout = 100;
            }
        }
        
        nabto_socket_t readySockets[16];
        uint16_t nReady = nabto_read_events(readySockets, 16, timeout);
        uint16_t i;
        for (i = 0; i < nReady; i++) {
            unabto_mutex_lock(&lock);
            unabto_read_socket(readySockets[i]);
            unabto_mutex_unlock(&lock);
            received = true;
        }

        if (nReady == 0) {
            unabto_mutex_lock(&lock);
            unabto_time_event();
            unabto_mutex_unlock(&lock);
        }
    }
    return 0;
}

unabto_api_status_t unabto_api_lock_take() {
    unabto_api_lock();
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_lock_release() {
    unabto_api_unlock();
    return UNABTO_API_OK;
}

void unabto_api_unlock() {
    unabto_mutex_unlock(&lock);
}

void unabto_api_lock() {
    unabto_mutex_lock(&lock);
}

unabto_mutex_t* unabto_api_get_mutex() {
    return &lock;
}
