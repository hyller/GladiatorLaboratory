/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_api_platform.h"

#include "FreeRTOS.h"
#include "task.h"

static bool running;

void unabto_tick_task();

/**
 * Start a freertos thread which runs the unabto platform until stop is called.
 */
unabto_api_status_t unabto_api_platform_start() {
    network_initialize();
    if (running) {
        return UNABTO_API_FAIL;
    }
    if (!unabto_init()) {
        return UNABTO_API_FAIL;
    }

    running = 1;
    xTaskCreate(unabto_tick_task, (signed portCHAR *)"Nabto ipstack task", 1024, NULL, 1, NULL);
}

void unabto_tick_task() {
    NABTO_LOG_TRACE(("unabto freertos tick task created"));
    uint32_t lastip = 0;
    vTaskDelay(100/portTICK_RATE_MS);
    uint32_t oldIp;
    while(running) {
        unabto_api_lock(); // needs to be synchronized since an
        network_tick();
        unabto_tick(&nmc);

        unabto_api_unlock();
        unabto_api_yield();
    }
}


unabto_api_status_t unabto_api_platform_stop() {
    unabto_close();
    running = false;
}
