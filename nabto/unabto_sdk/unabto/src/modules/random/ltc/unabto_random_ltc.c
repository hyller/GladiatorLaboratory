#include <tomcrypt.h>
#include <unabto/unabto_external_environment.h>

void nabto_random(uint8_t* buf, size_t len) {
    unsigned long bytes = rng_get_bytes(buf, len, NULL); 
    if (bytes != len) {
        NABTO_LOG_FATAL(("Random function did not give required bytes"));
    }
}
