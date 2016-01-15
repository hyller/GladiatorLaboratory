/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api_common.h"
#include "unabto_api.h"
#include "unabto/unabto_common_main.h"
#include "unabto/unabto_stream_event.h"
#include <unabto/unabto_stream.h>

unabto_api_status_t unabto_api_stream_read( 
    unabto_api_stream_t stream, uint8_t* buf, size_t count, size_t* read) {
#if NABTO_ENABLE_STREAM
    int hint;
    size_t readFromStream;
    do {
        unabto_api_lock();
        readFromStream = unabto_stream_read_buf(stream,buf,count,&hint);
        unabto_api_unlock();
        if (hint != 0) {
            NABTO_LOG_TRACE(("uNabto stream read failed code %i", hint));
            return UNABTO_API_FAIL;
        }
        // all ok 
        if (readFromStream > 0) {
            *read = readFromStream;
             return UNABTO_API_OK;
        }
        unabto_api_yield();
      
    } while (is_blocking());
    return UNABTO_API_WOULD_BLOCK;
#else
    return UNABTO_API_FAIL;
#endif
}

unabto_api_status_t unabto_api_stream_write(unabto_api_stream_t stream, uint8_t* buf, size_t size, size_t* written) {
#if NABTO_ENABLE_STREAM
    int hint;
    do {
        unabto_api_lock();
        *written = unabto_stream_write(stream, buf, size, &hint);
        unabto_api_unlock();
        if (hint) {
            NABTO_LOG_TRACE(("unabto_api_stream_write failed %i ", hint));
            return UNABTO_API_FAIL;
        }
        if (*written != 0 || size == 0) {
            return UNABTO_API_OK;
        }
        unabto_api_yield();
    } while (is_blocking());
    return UNABTO_API_WOULD_BLOCK;
#else
    return UNABTO_API_FAIL;
#endif
}

unabto_api_status_t unabto_api_stream_close(unabto_api_stream_t handle) {
#if NABTO_ENABLE_STREAM
    unabto_api_status_t ret;
    NABTO_LOG_TRACE(("unabto_api_stream_close"));
    do {
        unabto_api_lock();
        ret = unabto_stream_close(handle) ? UNABTO_API_OK : UNABTO_API_FAIL;
        unabto_api_unlock();

        if (ret == UNABTO_API_OK) {
            return ret;
        }

        unabto_api_yield();
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
    unabto_api_unlock();
    return UNABTO_API_OK;
#else
    return UNABTO_API_FAIL;
#endif
}
