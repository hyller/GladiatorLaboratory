#ifndef _TUNNEL_H_
#define _TUNNEL_H_

#include <unabto_platform_types.h>

void tunnel_loop();

/**
 * return false to disallow connections to the specified host:port
 */
bool tunnel_allow_connection(const char* host, int port);

#define DEFAULT_PORT 22
#define DEFAULT_HOST "127.0.0.1"

void tunnel_set_default_host(const char* host);
void tunnel_set_default_port(int port);

const char* tunnel_get_default_host();
int tunnel_get_default_port();

#endif // _TUNNEL_H_
