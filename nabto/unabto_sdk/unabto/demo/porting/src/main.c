#include <unabto/unabto_common_main.h>
#include <unabto/unabto_app.h>
int main() {
    nabto_main_setup* nms = unabto_init_context();
    nms->id = "myid.example.net";
    unabto_init();
    while(true) {
        unabto_tick();
    }
}

application_event_result application_event(application_request* request, buffer_read_t* read_buffer, buffer_write_t* write_buffer) {
    return AER_REQ_INV_QUERY_ID;
}

