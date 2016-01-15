/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api/unabto_api_platform.h"
#include "unabto_api/unabto_api_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "unabto/unabto_common_main.h"

static bool running;

void unabto_tick_task();

/**
 * Start a freertos thread which runs the unabto platform until stop is called.
 */
unabto_api_status_t unabto_api_platform_start() {
    if (running) {
        return UNABTO_API_FAIL;
    }
    if (!unabto_init()) {
        return UNABTO_API_FAIL;
    }

    running = 1;
    xTaskCreate(unabto_tick_task, (signed portCHAR *)"Nabto ipstack task", 1024, NULL, 1, NULL);
    return UNABTO_API_OK;
}

void unabto_tick_task() {
    while(running) {
        unabto_api_lock();
        unabto_tick();
        unabto_api_unlock();
        unabto_api_yield();
    }
    vTaskDelete(NULL);
    return;
}


unabto_api_status_t unabto_api_platform_stop() {
    running = false;
    return UNABTO_API_OK;
}
