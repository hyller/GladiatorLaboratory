/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifndef UNABTO_API_H
#define UNABTO_API_H

#ifdef WIN32
#ifndef NABTO_API_NO_INTTYPES
#define NABTO_API_NO_INTTYPES
#endif
#endif

/**
 * this unabto api exposes an unabto interface where we are not
 * calling external functions as eg. application_event. This makes it
 * easy to define a foreign function interface which just have to
 * import this interface.
 *
 * We assume mutexes and malloc/free
 */
#include <unabto/unabto_env_base.h>
#include <unabto/unabto_app.h>
#include <unabto/unabto_connection_type.h>
#include <unabto/unabto_stream.h>
#ifndef NABTO_API_NO_INTTYPES
#include <inttypes.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    UNABTO_API_OK,
    UNABTO_API_FAIL,
    UNABTO_API_WOULD_BLOCK,
    UNABTO_API_MALLOC_FAIL
} unabto_api_status_t;

typedef enum {
    UNABTO_API_STREAM_BLOCKING,
    UNABTO_API_STREAM_UNBLOCKING
} unabto_api_stream_option_t;

typedef enum {
    UNABTO_API_ATTACH_STATE_IDLE,
    UNABTO_API_ATTACH_STATE_WAIT_DNS,
    UNABTO_API_ATTACH_STATE_WAIT_BS,
    UNABTO_API_ATTACH_STATE_WAIT_GSP,
    UNABTO_API_ATTACH_STATE_ATTACHED
} unabto_api_attach_state_t;

struct nabto_main_context;
extern struct nabto_main_context nmc;

#if !NABTO_ENABLE_STREAM
#error Cannot compile unabto_api without streaming support
#endif

#if !NABTO_ENABLE_STREAM_EVENTS
#error Cannot compile unabto_api without NABTO_ENABLE_STREAM_EVENTS enabled.
#endif

#ifdef NABTO_API_NO_INTTYPES
typedef unabto_stream* unabto_api_stream_t;
#else
typedef unabto_stream* unabto_api_stream_t;
#endif

/**
 * A query handler consists of the four functions from unabto_app.h
 * look in the unabto core documentation for their semantics.
 */
typedef application_event_result (*application_event_t)(application_request* request, unabto_query_request* queryRequest, unabto_query_response* writeBuffer);
typedef bool (*application_poll_query_t)(application_request** applicationRequest);
typedef application_event_result (*application_poll_t)(application_request* applicationRequest, unabto_query_response* writeBuffer);
typedef void (*application_poll_drop_t)(application_request* applicationRequest);

unabto_api_status_t unabto_api_query_register_handler(
    application_event_t applicationEventHandler, 
    application_poll_query_t applicationPollQueryHandler,
    application_poll_t applicationPollHandler,
    application_poll_drop_t applicationPollDropHandler);

/**
 * Attach status callbacks
 */
typedef void (*unabto_api_attach_status_callback_t)(unabto_api_attach_state_t state, void* data);
unabto_api_status_t unabto_api_register_attach_state_change_callback(unabto_api_attach_status_callback_t cb, void* userData);


/**
 * Register a stream handler for new streams.
 */
typedef void (*unabto_api_stream_handler_t)(unabto_api_stream_t stream, void* data);
unabto_api_status_t unabto_api_stream_register_handler(unabto_api_stream_handler_t uash, void* data);

/**
 * Release a unabto stream this just destroys the stream, can be used if shutdown
 * fails. (like socket shutdown)
 */
unabto_api_status_t unabto_api_stream_release(unabto_api_stream_t handle);


/**
 * Read from a stream. The semantic is equivalent to the semantics of the
 * Berkeley streaming api.
 */
unabto_api_status_t unabto_api_stream_read(unabto_api_stream_t stream, uint8_t* buf, size_t count, size_t* read);

/**
 * Write to a stream. Semantics eqivalent to Berkeley sockets.
 */
unabto_api_status_t unabto_api_stream_write(unabto_api_stream_t stream, uint8_t* buf, size_t size, size_t* written);

/**
 * In addition to the normal read/write we also have _all functions which sends/receives
 * The exact amount of bytes requested
 */

/**
 * Read an exact amount of bytes from a stream, either return when the bytes are read
 * or fails for some reason if the amount of bytes could not be read.
 * @param stream  The stream to read the data from.
 * @param buf     The buffer with the read bytes.
 * @param count   The exact number of bytes to read.
 * @return UNABTO_API_OK on success.
 */
unabto_api_status_t unabto_api_stream_read_all(unabto_api_stream_t stream, uint8_t* buf, size_t count);

/**
 * Send an exact amount of bytes on the stream.
 * @param stream  The Stream to write the data on.
 * @param buf     The buffer holding the data to be sent.
 * @param count   The number of bytes to send.
 */
unabto_api_status_t unabto_api_stream_write_all(unabto_api_stream_t stream, uint8_t* buf, size_t count);

/**
 * Set an option on a global basis, this diverges from the normal posix behavior.
 * Where options is set on a stream.
 */
unabto_api_status_t unabto_api_setopt(unabto_api_stream_option_t option);

/**
 * Close a stream nicely.
 * If using blocking semantics the close will block until closed.
 *
 * If the stream is invalid UNABTO_API_FAIL is returned. 
 * Otherwise UNABTO_API_OK is returned on success. 
 */
unabto_api_status_t unabto_api_stream_close(unabto_api_stream_t handle);

typedef struct {
    nabto_connection_type connection_type;
    unsigned int sentPackets;
    unsigned int sentBytes;
    unsigned int sentResentPackets; // Packets which was sent before.
    unsigned int receivedPackets;
    unsigned int receivedBytes;
    unsigned int receivedResentPackets; // Packets which was received before.
    unsigned int reorderedOrLostPackets;
} unabto_api_stream_info_t;

/**
 * Get stream info.
 */
unabto_api_status_t unabto_api_stream_info(unabto_api_stream_t stream, unabto_api_stream_info_t* stream_info);

/**
 * Return true if we are blocking.
 */
bool is_blocking();

unabto_api_status_t unabto_api_eventchannel_send(char *event);

#ifdef __cplusplus
};
#endif


#endif /* UNABTO_API_H */
