/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api.h"
#include "unabto/unabto_main_contexts.h"
#include <unabto/unabto_stream.h>

static unabto_api_stream_option_t options = UNABTO_API_STREAM_BLOCKING;

typedef struct {
    application_event_t applicationEventHandler;
    application_poll_query_t applicationPollQueryHandler;
    application_poll_t applicationPollHandler;
    application_poll_drop_t applicationPollDropHandler;
} query_handler_t;

static query_handler_t queryHandler = {0,0,0,0};

static unabto_api_attach_status_callback_t unabto_attach_status_callback_func = 0;
static void*                               unabto_attach_status_callback_data;


nabto_main_context nmc;

bool is_blocking() {
    return options == UNABTO_API_STREAM_BLOCKING;
}

unabto_api_status_t unabto_api_setopt(unabto_api_stream_option_t option)
{
    options = option;
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_query_register_handler(
    application_event_t applicationEventHandler, 
    application_poll_query_t applicationPollQueryHandler,
    application_poll_t applicationPollHandler,
    application_poll_drop_t applicationPollDropHandler)
{
    queryHandler.applicationEventHandler = applicationEventHandler;
    queryHandler.applicationPollQueryHandler = applicationPollQueryHandler;
    queryHandler.applicationPollHandler = applicationPollHandler;
    queryHandler.applicationPollDropHandler = applicationPollDropHandler;
    return UNABTO_API_OK;
}

application_event_result application_event(application_request* request, buffer_read_t* b_r, buffer_write_t* b_w) {
    if (queryHandler.applicationEventHandler != NULL) {
        return queryHandler.applicationEventHandler(request, b_r, b_w);
    } else {
        return AER_REQ_NO_QUERY_ID;
    }
}

bool application_poll_query(application_request** appreq) {
    if (queryHandler.applicationPollQueryHandler != NULL) {
        return queryHandler.applicationPollQueryHandler(appreq);
    } else {
        return false;
    }
}

application_event_result application_poll(application_request* appreq, buffer_read_t* b_r, buffer_write_t* b_w) {
    if (queryHandler.applicationPollHandler != NULL) {
        return queryHandler.applicationPollHandler(appreq, b_w);
    } else {
        return AER_REQ_SYSTEM_ERROR;
    }
}

void application_poll_drop(application_request* appreq) { 
    if (queryHandler.applicationPollDropHandler != NULL) {
        queryHandler.applicationPollDropHandler(appreq);
    }
}



unabto_api_status_t unabto_api_register_attach_state_change_callback(unabto_api_attach_status_callback_t cb, void* userData) {
    unabto_attach_status_callback_func = cb;
    unabto_attach_status_callback_data = userData;
    return UNABTO_API_OK;
}

void unabto_attach_state_changed(nabto_state state) {
    if (unabto_attach_status_callback_func != 0) {
        unabto_api_attach_state_t st = (unabto_api_attach_state_t)state;
        unabto_attach_status_callback_func(st, unabto_attach_status_callback_data);
    }
}

unabto_api_status_t unabto_api_stream_write_all(unabto_api_stream_t stream, uint8_t* buf, size_t count) {
    size_t bytes_written = 0;
    size_t delta;
    NABTO_LOG_TRACE(("unabto_api_stream_write_all"));
    do {
        if (unabto_api_stream_write(stream, buf+bytes_written, count-bytes_written, &delta) != UNABTO_API_OK) {
            return UNABTO_API_FAIL;
        }
        bytes_written += delta;
    } while (bytes_written < count);
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_stream_read_all(unabto_api_stream_t stream, uint8_t* buf, size_t count) {
    size_t bytes_read = 0;
    size_t delta;
    do {
        if (unabto_api_stream_read(stream, buf+bytes_read, count-bytes_read, &delta) != UNABTO_API_OK) {
            return UNABTO_API_FAIL;
        }
        bytes_read +=  delta;
    } while (bytes_read < count);
    return UNABTO_API_OK;
}


