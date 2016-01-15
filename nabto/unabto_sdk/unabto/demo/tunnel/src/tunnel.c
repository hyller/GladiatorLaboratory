#include <unabto/unabto_stream.h>
#include <unabto/unabto_app.h>
#include <unabto/unabto_common_main.h>
#include <unabto/unabto_main_contexts.h>
#include <unabto/unabto_external_environment.h>
#include <modules/timers/auto_update/unabto_time_auto_update.h>

#include <modules/network/select/unabto_network_select_api.h>
#include <modules/tcp_fallback/tcp_fallback_select.h>

#include <string.h>

#if defined(WIN32) || defined(WINCE)
// use winsock
#define WINSOCK 1
#else
// use a bsd api
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "tunnel.h"

#if defined(WINSOCK)
#define SHUT_WR SD_BOTH
#define MSG_NOSIGNAL 0
#define _SCL_SECURE_NO_WARNINGS
typedef SOCKET tunnelSocket;
#define close closesocket
typedef int optlen;
#else
typedef int tunnelSocket;
typedef socklen_t optlen;
#define INVALID_SOCKET -1
#endif

#ifdef __MACH__
#define MSG_NOSIGNAL 0
#endif


#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef enum {
    TS_IDLE,
    TS_READ_COMMAND,
    TS_PARSE_COMMAND,
    TS_OPEN_SOCKET,
    TS_OPENING_SOCKET,
    TS_FORWARD,
    TS_CLOSING
} tunnelStates;

typedef enum {
    FS_READ,
    FS_WRITE,
    FS_CLOSING
} forwardState;

typedef enum {
    TUNNEL_EVENT_SOURCE_TCP_WRITE,
    TUNNEL_EVENT_SOURCE_TCP_READ,
    TUNNEL_EVENT_SOURCE_UNABTO
} tunnel_event_source;

#define MAX_COMMAND_LENGTH 512
#define MAX_HOST_LENGTH 128

typedef struct tunnel {
    unabto_stream* stream;
    tunnelStates state;
    uint8_t command[MAX_COMMAND_LENGTH];
    int commandLength;
    char host[MAX_HOST_LENGTH];
    int port;
    tunnelSocket sock;
    
    forwardState tcpReadState;
    forwardState unabtoReadState;

    uint8_t tcpReadBuffer[NABTO_STREAM_SEND_SEGMENT_SIZE];
    int tcpReadBufferSize;
    int tcpReadBufferSent;
    int tunnelId;
} tunnel;

NABTO_THREAD_LOCAL_STORAGE tunnel tunnels[NABTO_STREAM_MAX_STREAMS];

static int tunnelCounter = 0;

void tunnel_event(tunnel* state, tunnel_event_source event_source);

bool open_socket(tunnel* tunnel);

void tcp_forward(tunnel* tunnel);
void unabto_forward(tunnel* tunnel);

bool add_socket(int sock);

void remove_socket(int sock);

void tunnel_event_socket(int socket);
bool parse_command(tunnel* tunnel);
bool opening_socket(tunnel* tunnel);

void reset_tunnel_struct(tunnel* t);

void tunnel_loop();

void tunnel_loop() {
    int i;
    for (i = 0; i < NABTO_STREAM_MAX_STREAMS; i++) {
        reset_tunnel_struct(&tunnels[i]);
    }

    if (!unabto_init()) {
        NABTO_LOG_FATAL(("Failed to initialize unabto"));
    }
    unabto_time_auto_update(false);

    while(true) {
        nabto_stamp_t nextEvent;
        nabto_stamp_t now;
        int timeout;
        int i;
        fd_set read_fds;
        fd_set write_fds;
        int max_read_fd = 0;
        int max_write_fd = 0;
        struct timeval timeout_val;
        int nfds;

        unabto_time_update_stamp();
        unabto_next_event(&nextEvent);
        now = nabtoGetStamp();
        timeout = nabtoStampDiff2ms(nabtoStampDiff(&nextEvent, &now));

        if (timeout < 0) {
            timeout = 1;
        }

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        unabto_network_select_add_to_read_fd_set(&read_fds, &max_read_fd);

#if NABTO_ENABLE_TCP_FALLBACK
        unabto_tcp_fallback_select_add_to_read_fd_set(&read_fds, &max_read_fd);
        unabto_tcp_fallback_select_add_to_write_fd_set(&write_fds, &max_write_fd);
#endif

        for (i = 0; i < NABTO_STREAM_MAX_STREAMS; i++) {
            if (tunnels[i].state != TS_IDLE) {
                if (tunnels[i].state == TS_FORWARD && tunnels[i].tcpReadState == FS_READ) {
                    FD_SET(tunnels[i].sock, &read_fds);
                    max_read_fd = MAX(max_read_fd, tunnels[i].sock);
                }
                if ((tunnels[i].state == TS_FORWARD && tunnels[i].unabtoReadState == FS_WRITE) ||
                    tunnels[i].state == TS_OPENING_SOCKET) {
                    FD_SET(tunnels[i].sock, &write_fds);
                    max_write_fd = MAX(max_write_fd, tunnels[i].sock);
                }
            }
        }
        timeout_val.tv_sec = (timeout/1000);
        timeout_val.tv_usec = ((timeout)%1000)*1000;

        nfds = select(MAX(max_read_fd+1, max_write_fd+1), &read_fds, &write_fds, NULL, &timeout_val);

        if (nfds < 0) {
#if defined(WINSOCK)
            if (WSAGetLastError() == WSAEINTR) {
                // ok
            } else {
                NABTO_LOG_ERROR(("Select returned error %d", WSAGetLastError()));
                return;
            }
#else
            int err = errno;
            if (err == EINTR) {
                // ok
            } else {
                NABTO_LOG_ERROR(("Select returned error %s, %i", strerror(err), err));
                return;
            }
#endif
        }

        unabto_time_update_stamp();

        if (nfds > 0) {
#if NABTO_ENABLE_TCP_FALLBACK
            unabto_tcp_fallback_select_write_sockets(&write_fds);
            unabto_tcp_fallback_select_read_sockets(&read_fds);
#endif
            unabto_network_select_read_sockets(&read_fds);
            for (i = 0; i < NABTO_STREAM_MAX_STREAMS; i++) {
                if (tunnels[i].sock != INVALID_SOCKET && FD_ISSET(tunnels[i].sock, &read_fds)) {
                    tunnel_event(&tunnels[i], TUNNEL_EVENT_SOURCE_TCP_READ);
                }
                if (tunnels[i].sock != INVALID_SOCKET && FD_ISSET(tunnels[i].sock, &write_fds)) {
                    tunnel_event(&tunnels[i], TUNNEL_EVENT_SOURCE_TCP_WRITE);
                }
            }
        }
        unabto_time_event();
    }
    unabto_close();
}

void unabto_stream_accept(unabto_stream* stream) {
    tunnel* t = &tunnels[unabto_stream_index(stream)];
    NABTO_LOG_TRACE(("Accepting stream and assigning it to tunnel %i", t));
    UNABTO_ASSERT(t->state == TS_IDLE);
    reset_tunnel_struct(t);

    t->stream = stream;
    t->state = TS_READ_COMMAND;
    t->tunnelId = tunnelCounter++;
}

void unabto_stream_event(unabto_stream* stream, unabto_stream_event_type event) {
    tunnel* state;
    NABTO_LOG_TRACE(("Stream %i, %i", unabto_stream_index(stream), event));
    state = &tunnels[unabto_stream_index(stream)];
    tunnel_event(state, TUNNEL_EVENT_SOURCE_UNABTO);
}

void tunnel_event(tunnel* tunnel, tunnel_event_source event_source) {

    NABTO_LOG_TRACE(("Tunnel event on tunnel %i", tunnel));
    if (tunnel->state == TS_IDLE) {
        NABTO_LOG_ERROR(("Tunnel(%i), Event on tunnel which should not be in IDLE state. source %i, tcpReadState %i, unabtoReadState %i, stream index %i, socket %i", tunnel->tunnelId, event_source, tunnel->tcpReadState, tunnel->unabtoReadState, unabto_stream_index(tunnel->stream), tunnel->sock));
        return;
    }

    if (tunnel->state == TS_READ_COMMAND) {
        const uint8_t* buf;
        unabto_stream_hint hint;
        size_t readen = unabto_stream_read(tunnel->stream, &buf, &hint);
        if (hint != UNABTO_STREAM_HINT_OK) {
            NABTO_LOG_TRACE(("Releasing stream in state TS_READ_COMMAND"));
            tunnel->state = TS_CLOSING;
        } else {
            if (readen > 0) {
                size_t i;
                int ackSize = 0;
                for (i = 0; i < readen; i++) {
                    if (buf[i] == '\n') {
                        tunnel->state = TS_PARSE_COMMAND;
                    } else {
                        tunnel->command[tunnel->commandLength] = buf[i];
                        tunnel->commandLength++;
                    }
                    
                    if (tunnel->commandLength > MAX_COMMAND_LENGTH) {
                        NABTO_LOG_ERROR(("Tunnel command too long"));
                        tunnel->state = TS_CLOSING;
                    }
                }
                
                unabto_stream_ack(tunnel->stream, buf, i, &hint);

                if (hint != UNABTO_STREAM_HINT_OK) {
                    NABTO_LOG_ERROR(("Failed to ack on stream."));
                    tunnel->state = TS_CLOSING;
                }
            }
        }
    }

    if (tunnel->state == TS_PARSE_COMMAND) {
        if (parse_command(tunnel)) {
            if (tunnel_allow_connection(tunnel->host, tunnel->port)) {
                tunnel->state = TS_OPEN_SOCKET;
                NABTO_LOG_INFO(("Tunnel(%i) connecting to %s:%i", tunnel->tunnelId, tunnel->host, tunnel->port));
            } else {
                NABTO_LOG_ERROR(("Tunnel(%i) not allowed to connect to %s:%i", tunnel->tunnelId, tunnel->host, tunnel->port));
                tunnel->state = TS_CLOSING;
            }
        } else {
            NABTO_LOG_ERROR(("Tunnel(%i) Could not parse tunnel command %s", tunnel->tunnelId, tunnel->command));
            tunnel->state = TS_CLOSING;
            
        }
    }

    if (tunnel->state == TS_OPENING_SOCKET && event_source == TUNNEL_EVENT_SOURCE_TCP_WRITE) {
        if (!opening_socket(tunnel)) {
            tunnel->state = TS_CLOSING;
        }
    }

    if (tunnel->state == TS_OPEN_SOCKET) {
        // TODO make async
        if (!open_socket(tunnel)) {
            tunnel->state = TS_CLOSING;
        }
    }

    

    if (tunnel->state == TS_FORWARD) {
        if (tunnel->tcpReadState == FS_CLOSING && tunnel->unabtoReadState == FS_CLOSING) {
            tunnel->state = TS_CLOSING;
        } else {
            tcp_forward(tunnel);
            unabto_forward(tunnel);
        }
    }

    if (tunnel->tcpReadState == FS_CLOSING && tunnel->unabtoReadState == FS_CLOSING) {
        tunnel->state = TS_CLOSING;
    }


    if (tunnel->state == TS_CLOSING) {
        
        const uint8_t* buf;
        unabto_stream_hint hint;
        size_t readen;

        do {
            readen = unabto_stream_read(tunnel->stream, &buf, &hint);
            if (readen > 0) {
                unabto_stream_ack(tunnel->stream, buf, readen, &hint);
            }
        } while (readen > 0);

        if (unabto_stream_close(tunnel->stream)) {
            unabto_stream_stats info;
            unabto_stream_get_stats(tunnel->stream, &info);

            NABTO_LOG_TRACE(("Closed tunnel successfully"));
            NABTO_LOG_INFO(("Tunnel(%i) closed, sentPackets: %u, sentBytes %u, sentResentPackets %u, receivedPackets %u, receivedBytes %u, receivedResentPackets %u, reorderedOrLostPackets %u", 
                            tunnel->tunnelId,
                            info.sentPackets, info.sentBytes, info.sentResentPackets,
                            info.receivedPackets, info.receivedBytes, info.receivedResentPackets, info.reorderedOrLostPackets));

            close(tunnel->sock);
            unabto_stream_release(tunnel->stream);
            reset_tunnel_struct(tunnel);
        }
    }
}

#define PORT_KW_TXT "port="
#define HOST_KW_TXT "host="

bool parse_command(tunnel* tunnel) {
    
    char* s;

    if (NULL != (s = strstr((const char*)tunnel->command, PORT_KW_TXT)))
    {
        s += strlen(PORT_KW_TXT);
        if (1 != sscanf(s, "%d", &tunnel->port)) {
            NABTO_LOG_ERROR(("failed to read port number"));
            return false;
        }
    } else {
        tunnel->port = tunnel_get_default_port();
    }
    
    if (NULL != (s = strstr((const char*)tunnel->command, HOST_KW_TXT)))
    {
        char *sp;
        int length;
        s += strlen(HOST_KW_TXT);
        sp = strchr(s, ' ');
        
        if (sp != NULL) {
            length = sp-s;
        } else {
            length = strlen(s);
        }
        
        strncpy(tunnel->host, s, MIN(length, MAX_COMMAND_LENGTH-1));
    } else {
        strncpy(tunnel->host, tunnel_get_default_host(), MAX_HOST_LENGTH);
    }
    
    return true;
}


void close_tcp_reader(tunnel* tunnel) {
    unabto_stream_close(tunnel->stream);
    tunnel->tcpReadState = FS_CLOSING;
}

/**
 * read from tcp, write to unabto
 */
void tcp_forward(tunnel* tunnel) {
    while(true) {
        if (tunnel->tcpReadState == FS_READ) {
            ssize_t readen = recv(tunnel->sock, tunnel->tcpReadBuffer, NABTO_STREAM_SEND_SEGMENT_SIZE, 0);
            if (readen == 0) {
                // eof
                close_tcp_reader(tunnel);
                break;
            }

            if (readen > 0) {
                tunnel->tcpReadBufferSize = readen;
                tunnel->tcpReadBufferSent = 0;
                tunnel->tcpReadState = FS_WRITE;
            }

            if (readen < 0) {
#if defined(WINSOCK)
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    break;
#else
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
#endif
                } else {
                    close_tcp_reader(tunnel);
                    break;
                }
            }
        }

        if (tunnel->tcpReadState == FS_WRITE) {
            unabto_stream_hint hint;
            size_t written = unabto_stream_write(tunnel->stream, tunnel->tcpReadBuffer+tunnel->tcpReadBufferSent, tunnel->tcpReadBufferSize - tunnel->tcpReadBufferSent, &hint);
            if (hint != UNABTO_STREAM_HINT_OK) {
                NABTO_LOG_TRACE(("Can't write to stream"));
                close_tcp_reader(tunnel);
                break;
            } else {
                if (written > 0) {
                    tunnel->tcpReadBufferSent += written;
                    if (tunnel->tcpReadBufferSize == tunnel->tcpReadBufferSent) {
                        tunnel->tcpReadState = FS_READ;
                    }
                } else {
                    break;
                }
            }
        }

        if (tunnel->tcpReadState == FS_CLOSING) {
            break;
        }
    }
}

void close_stream_reader(tunnel* tunnel) {
    tunnel->unabtoReadState = FS_CLOSING;
    shutdown(tunnel->sock, SHUT_WR);
}

/**
 * read from unabto, write to tcp
 */
void unabto_forward(tunnel* tunnel) {
    if (tunnel->unabtoReadState == FS_WRITE) {
        tunnel->unabtoReadState = FS_READ;
    }
    while(true) {
        if (tunnel->unabtoReadState == FS_READ) {
            const uint8_t* buf;
            unabto_stream_hint hint;
            size_t readen = unabto_stream_read(tunnel->stream, &buf, &hint);
            if (hint != UNABTO_STREAM_HINT_OK) {
                close_stream_reader(tunnel);
                break;
            } else {
                if (readen == 0) {
                    break;
                } else {
                    ssize_t written;
                    NABTO_LOG_TRACE(("Write to tcp stream %i", readen));
                    written = send(tunnel->sock, buf, readen, MSG_NOSIGNAL);

                    if (written > 0) {
                        NABTO_LOG_TRACE(("Wrote to tcp stream %i", written));
                        unabto_stream_ack(tunnel->stream, buf, written, &hint);
                        if (hint != UNABTO_STREAM_HINT_OK) {
                            close_stream_reader(tunnel);
                            break;
                        }
                    } else if (written == 0) {
                        tunnel->unabtoReadState = FS_WRITE;
                        break;
                    } else { // -1
#if defined(WINSOCK)
                        NABTO_LOG_TRACE(("Wrote to tcp stream %i, status %d", written, WSAGetLastError()));
#else
                        NABTO_LOG_TRACE(("Wrote to tcp stream %i, status %s", written, strerror(errno)));
#endif

#if defined(WINSOCK)
                        if (WSAGetLastError() == WSAEWOULDBLOCK) {
                            tunnel->unabtoReadState = FS_WRITE;
                            break;
#else
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            tunnel->unabtoReadState = FS_WRITE;
                            break;
#endif
                        } else {
                            close_stream_reader(tunnel);
                            break;
                        }
                    }
                }
            }
        }

        if (tunnel->unabtoReadState == FS_WRITE) {
            break;
        }

        if (tunnel->unabtoReadState == FS_CLOSING) {
            break;
        }
    }
}

bool opening_socket(tunnel* tunnel) {
    char err;
    optlen len;
    int status;
    len = sizeof(err);
    status = getsockopt(tunnel->sock, SOL_SOCKET, SO_ERROR, &err, &len);
    if (status != 0) {
#if defined(WINSOCK)
        NABTO_LOG_ERROR(("Error opening socket %d", WSAGetLastError()));
#else
        NABTO_LOG_ERROR(("Error opening socket %s", strerror(errno)));
#endif
        return false;
    } else {
        if (err == 0) {
            tunnel->state = TS_FORWARD;
        } else {
#if defined(WINSOCK)
	    NABTO_LOG_ERROR(("Error opening socket %s", WSAGetLastError())); 
#else	  
            NABTO_LOG_ERROR(("Error opening socket %s", strerror(err)));
#endif
            return false;
        }
    }
    return true;
}

bool open_socket(tunnel* tunnel) {
    struct sockaddr_in sin;
    if (-1 == (tunnel->sock = socket(AF_INET, SOCK_STREAM, 0))) {
        NABTO_LOG_ERROR(("Failed to create socket"));
        return false;
    }

// On windows we make a blocking accept since a nonblocking accept fails.
#ifndef WINSOCK
    {
        int flags = fcntl(tunnel->sock, F_GETFL, 0);
        if (flags < 0) return false;
        flags = (flags|O_NONBLOCK);
        if (fcntl(tunnel->sock, F_SETFL, flags) != 0) {
            NABTO_LOG_FATAL(("Cannot set unblocking mode"));
            return false;
        }
    }
#endif

    memset(&sin, 0, sizeof (struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(tunnel->host);
    sin.sin_port = htons(tunnel->port);
    if (-1 == connect(tunnel->sock, (struct sockaddr*) &sin, sizeof (struct sockaddr_in))) {
#if defined(WINSOCK)
        int err = WSAGetLastError();
        if (err == WSAEINPROGRESS || err == WSAEWOULDBLOCK) {
            tunnel->state = TS_OPENING_SOCKET;
        } else {
            NABTO_LOG_ERROR(("connect failed: %d", WSAGetLastError()));
            return false;
        }
#else
        int err = errno;
        if (err == EINPROGRESS || err == EWOULDBLOCK) {
            tunnel->state = TS_OPENING_SOCKET;
        } else {
            NABTO_LOG_ERROR(("connect failed: %s", strerror(err)));
            return false;
        }
#endif
    } else {
        tunnel->state = TS_FORWARD;
    }

#ifdef WINSOCK
    {
        unsigned long flags = 1;
        if (ioctlsocket(tunnel->sock, FIONBIO, &flags) != 0) {
            NABTO_LOG_ERROR(("Cannot set unblocking mode"));        
            return false;
        }
    }
#endif

    return true;
}

void reset_tunnel_struct(tunnel* t) {
    memset(t, 0, sizeof(struct tunnel));
    t->sock = INVALID_SOCKET;
}

 
const char* tunnel_host = DEFAULT_HOST;
int tunnel_port = DEFAULT_PORT;


void tunnel_set_default_host(const char* host) {
    tunnel_host = host;
}

void tunnel_set_default_port(int port) {
    tunnel_port = port;
}

const char* tunnel_get_default_host() {
    return tunnel_host;
}

int tunnel_get_default_port() {
    return tunnel_port;
}
