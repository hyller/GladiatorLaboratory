/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include <unabto_api/unabto_api.h>
#include <unabto_api/unabto_api_internal.h>
#include <unabto/unabto_stream.h>
#include <unabto/unabto_stream_event.h>

unabto_api_status_t unabto_api_stream_info(unabto_api_stream_t stream, unabto_api_stream_info_t* stream_info) {
#if NABTO_ENABLE_STREAM
    int hint;
    unabto_stream_stats stats;
    unabto_api_lock_take();
    hint = unabto_stream_get_connection_type(stream, &stream_info->connection_type);
    if (hint != 0) {
        unabto_api_lock_release();
        return UNABTO_API_FAIL;
    }
    hint = unabto_stream_get_stats(stream, &stats);
    if (hint != 0) {
        unabto_api_lock_release();
        return UNABTO_API_FAIL;
    }
    stream_info->sentPackets            = stats.sentPackets;
    stream_info->sentBytes              = stats.sentBytes;
    stream_info->sentResentPackets      = stats.sentResentPackets;
    stream_info->receivedPackets        = stats.receivedPackets;
    stream_info->receivedBytes          = stats.receivedBytes;
    stream_info->receivedResentPackets  = stats.receivedResentPackets;
    stream_info->reorderedOrLostPackets = stats.reorderedOrLostPackets;
    unabto_api_lock_release();
    return UNABTO_API_OK;
#else
    return UNABTO_API_FAIL;
#endif
}
