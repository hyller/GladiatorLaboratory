/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api.h"
#include "unabto_api_test.h"
#include "unabto_weather_station.h"
#include "unabto_api_test_application.h"

#include "unabto_api_platform.h"

#include <pthread.h>

void handle_stream(unabto_api_stream_t stream) {
    pthread_t thread;
    pthread_create(&thread, NULL, &echo, (void*)stream);
}

int main(int argc, char** argv) {
    
    unabto_api_platform_set_name("42.weather.u.nabto.net");
    unabto_api_platform_start();
    
    unabto_api_register_query_handler(&app);
    for (;;) {
        unabto_api_stream_t stream;
        if (unabto_api_stream_accept(&stream) == UNABTO_API_OK) {
            handle_stream(stream);
        }
        sched_yield();
    }
    unabto_api_platform_stop();
    return 0;
}

#define BUFSIZE 1500

/** 
 * Echo using _all stream functionality.
 */
void *echo(void* params) {
    unabto_api_stream_t stream;
    memcpy(&stream, params, sizeof(unabto_api_stream_t));
    uint8_t buf[BUFSIZE];
    size_t bytes_read;
    for(;;) {
        if (unabto_api_stream_read(stream, buf, BUFSIZE, &bytes_read) != UNABTO_API_OK) {
            NABTO_LOG_INFO(("stream read failed"));
            break;
        }
        if (unabto_api_stream_write_all(stream, buf, bytes_read) != UNABTO_API_OK) {
            NABTO_LOG_INFO(("stream write failed"));
            break;
        }
    }
    return NULL;
}
