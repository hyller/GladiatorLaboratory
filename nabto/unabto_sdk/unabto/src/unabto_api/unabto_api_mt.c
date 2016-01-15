/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api_common.h"
#include "unabto_api.h"
#include "unabto_api_internal.h"
#include "unabto/unabto_common_main.h"
#include "unabto/unabto_stream_event.h"
#include <unabto/unabto_stream.h>
#include <modules/mt/unabto_cond.h>

typedef struct {
    unabto_cond_t readCondition;
    unabto_cond_t writeCondition;
    unabto_cond_t closedCondition;
} streamMutexes;

streamMutexes streamState[NABTO_STREAM_MAX_STREAMS];

static unabto_api_stream_handler_t unabto_api_stream_handler_func = 0;
static void*                       unabto_api_stream_handler_data;

void unabto_stream_event(unabto_stream* stream, unabto_stream_event_type event) {
    streamMutexes* sm = &streamState[unabto_stream_index(stream)];
    switch (event) {
        case UNABTO_STREAM_EVENT_TYPE_DATA_READY:
            unabto_cond_signal(&sm->readCondition);
            break;
        case UNABTO_STREAM_EVENT_TYPE_DATA_WRITTEN:
            unabto_cond_signal(&sm->writeCondition);
            break;
        case UNABTO_STREAM_EVENT_TYPE_WRITE_CLOSED:
            unabto_cond_signal(&sm->writeCondition);
            break;
        case UNABTO_STREAM_EVENT_TYPE_READ_CLOSED:
            unabto_cond_signal(&sm->readCondition);
            break;
        case UNABTO_STREAM_EVENT_TYPE_CLOSED:
            unabto_cond_signal(&sm->readCondition);
            unabto_cond_signal(&sm->writeCondition);
            unabto_cond_signal(&sm->closedCondition);
            break;
    }
}

unabto_api_status_t unabto_api_stream_register_handler(unabto_api_stream_handler_t uash, void* data) {
    unabto_api_stream_handler_func = uash;
    unabto_api_stream_handler_data = data;
    return UNABTO_API_OK;
}

void initStreamState(unabto_stream* stream) {
    streamMutexes* sm = &streamState[unabto_stream_index(stream)];
    unabto_cond_init(&sm->readCondition);
    unabto_cond_init(&sm->writeCondition);
    unabto_cond_init(&sm->closedCondition);
}

void destroyStreamState(unabto_stream* stream) {
    streamMutexes* sm = &streamState[unabto_stream_index(stream)];
    unabto_cond_broadcast(&sm->readCondition);
    unabto_cond_destroy(&sm->readCondition);
    unabto_cond_broadcast(&sm->writeCondition);
    unabto_cond_destroy(&sm->writeCondition);
    unabto_cond_broadcast(&sm->closedCondition);
    unabto_cond_destroy(&sm->closedCondition);
}

void unabto_stream_accept(unabto_stream* stream) {
    NABTO_LOG_INFO(("accepting stream"));
    initStreamState(stream);
    if (unabto_api_stream_handler_func) {
        unabto_api_stream_handler_func(stream, unabto_api_stream_handler_data);
    } else {
        destroyStreamState(stream);
        unabto_stream_release(stream);
    }
}

unabto_api_status_t unabto_api_stream_read( 
    unabto_api_stream_t stream, uint8_t* buf, size_t count, size_t* read) {
#if NABTO_ENABLE_STREAM
    unabto_stream_hint hint;
    size_t readFromStream;
    do {
        unabto_api_lock();
        readFromStream = unabto_stream_read_buf(stream,buf,count,&hint);
        if (hint != UNABTO_STREAM_HINT_OK) {
            NABTO_LOG_TRACE(("uNabto stream read failed code %i", hint));
            unabto_api_unlock();
            return UNABTO_API_FAIL;
        }
        // all ok 
        if (readFromStream > 0) {
            *read = readFromStream;
            unabto_api_unlock();
            return UNABTO_API_OK;
        }
        if (is_blocking()) {
            unabto_cond_wait(&streamState[unabto_stream_index(stream)].readCondition, unabto_api_get_mutex());
        }
        unabto_api_unlock();
    } while (is_blocking());
    return UNABTO_API_WOULD_BLOCK;
#else
    return UNABTO_API_FAIL;
#endif
}

unabto_api_status_t unabto_api_stream_write(unabto_api_stream_t stream, uint8_t* buf, size_t size, size_t* written) {
#if NABTO_ENABLE_STREAM
  unabto_stream_hint hint;
    do {
        unabto_api_lock();
        *written = unabto_stream_write(stream, buf, size, &hint);
        if (hint) {
            NABTO_LOG_TRACE(("unabto_api_stream_write failed %i ", hint));
            unabto_api_unlock();
            return UNABTO_API_FAIL;
        } else if (*written != 0 || size == 0) {
            unabto_api_unlock();
            return UNABTO_API_OK;
        } else if (is_blocking()) {
            unabto_cond_wait(&streamState[unabto_stream_index(stream)].writeCondition, unabto_api_get_mutex());
        }
        unabto_api_unlock();
    } while (is_blocking());
    return UNABTO_API_WOULD_BLOCK;
#else
    return UNABTO_API_FAIL;
#endif
}

unabto_api_status_t unabto_api_stream_close(unabto_api_stream_t stream) {
#if NABTO_ENABLE_STREAM
    unabto_api_status_t ret;
    NABTO_LOG_TRACE(("unabto_api_stream_close"));
    do {
        unabto_api_lock();
        ret = unabto_stream_close(stream) ? UNABTO_API_OK : UNABTO_API_FAIL;
        if (ret == UNABTO_API_OK) {
            unabto_api_unlock();
            return ret;
        }
        if (is_blocking()) {
            unabto_cond_wait(&streamState[unabto_stream_index(stream)].writeCondition, unabto_api_get_mutex());
        }
        unabto_api_unlock();
        
    } while (is_blocking());
    return UNABTO_API_WOULD_BLOCK;
#else
    return UNABTO_API_FAIL;
#endif
}

unabto_api_status_t unabto_api_stream_release(unabto_api_stream_t stream) {
#if NABTO_ENABLE_STREAM
    unabto_api_lock();
    unabto_stream_release(stream);
    destroyStreamState(stream);
    unabto_api_unlock();
    return UNABTO_API_OK;
#else
    return UNABTO_API_FAIL;
#endif
}
