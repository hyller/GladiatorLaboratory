/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api.h"
#include "unabto_api_test_application.h"

void app(application_request* request, buffer_t* read_buffer) {
    buffer_t buffer; // = (buffer_t*)malloc(sizeof(buffer_t));
    buffer_init(&buffer, (uint8_t*)malloc(4), 4);

    buffer_write_t bw;
    buffer_write_init(&bw, &buffer);
    buffer_write_uint32(&bw, 42);

    unabto_api_query_response(request, AER_REQ_RESPONSE_READY, buffer);
}
