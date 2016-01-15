#include <unabto_platform_types.h>
#include <unabto/unabto_common_main.h>
#include <unabto/unabto_app.h>
#include "tunnel.h"

/**
 * This is a simple example of a custom tunnel implementation.
 */
int main() {
    nabto_main_setup* nms = unabto_init_context();

    // Setup device id.
    nms->id = "deviceid";
    
    // Setup encryption.
    nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;
    nms->secureAttach = true;
    nms->secureData = true;
    // Copy the preshared key into unabto
    // memcpy(nms->presharedKey, key,16);
    
    // Loop forever.
    tunnel_loop();
}

bool tunnel_allow_connection(const char* host, int port) {
    return true;
}

application_event_result application_event(application_request* request, buffer_read_t* readBuffer, buffer_write_t* writeBuffer)
{
    return AER_REQ_INV_QUERY_ID;
}

bool application_poll_query(application_request** applicationRequest) {
    return false;
}

application_event_result application_poll(application_request* applicationRequest, buffer_read_t* readBuffer, buffer_write_t* writeBuffer) {
    return AER_REQ_INV_QUERY_ID;
}

void application_poll_drop(application_request* applicationRequest) {
}
