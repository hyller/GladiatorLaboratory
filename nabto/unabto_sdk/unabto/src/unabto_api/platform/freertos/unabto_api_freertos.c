/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
/**
 * Define unabto freertos specific functionality
 */

#include "unabto_api/unabto_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "unabto_api/unabto_api_common.h"

#include "unabto/unabto_main_contexts.h"
#include "unabto/unabto_stream.h"
#include "unabto_api/unabto_api_internal.h"
#include "unabto/unabto_common_main.h"
static xSemaphoreHandle lock;

static nabto_main_setup* setup;

unabto_api_status_t unabto_api_platform_init() {
    lock = xSemaphoreCreateMutex();
    setup = unabto_init_context();
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_platform_set_name(const char* n)  {
    setup->id = strdup(n);
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_platform_set_controller_address(nabto_endpoint controllerEp) {
    setup->controllerArg = controllerEp;
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_internal_get_context(nabto_main_setup** s) {
    *s = setup;
    return UNABTO_API_OK;
}

void unabto_api_lock() {
    xSemaphoreTake(lock, portMAX_DELAY);
}

void unabto_api_unlock() {
    xSemaphoreGive(lock);
}

unabto_api_status_t unabto_api_lock_take() {
    unabto_api_lock();
    return UNABTO_API_OK;
}

unabto_api_status_t unabto_api_lock_release() {
    unabto_api_unlock();
    return UNABTO_API_OK;
}

/* void unabto_api_mutex_destroy() { */
/*     vSemaphoreDelete(lock); */
/* } */

void unabto_api_yield() {
    vTaskDelay(1/portTICK_RATE_MS);
}
