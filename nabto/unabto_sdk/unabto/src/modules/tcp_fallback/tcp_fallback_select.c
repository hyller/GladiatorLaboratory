#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#else
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/tcp.h>
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <modules/tcp_fallback/tcp_fallback_select.h>
#include <unabto/unabto_util.h>
#include <errno.h>
#include <unabto/unabto_memory.h>
#include <unabto/unabto_message.h>
#include <unabto/unabto_tcp_fallback.h>

#ifdef WIN32
#define SHUT_RDWR SD_BOTH
#define write(s,d,l) _write(s,d,l)
#define MSG_NOSIGNAL 0
typedef int optlen;
#else
#define INVALID_SOCKET (-1)
#define closesocket(s) close(s)
typedef socklen_t optlen;
#endif

#ifdef __MACH__
#define MSG_NOSIGNAL 0
#endif

static void tcp_fallback_connection_lost(nabto_connect* con);

static NABTO_THREAD_LOCAL_STORAGE unabto_tcp_fallback_connection fbConns[NABTO_CONNECTIONS_SIZE];

void unabto_tcp_fallback_handle_connect(nabto_connect* con);
void unabto_tcp_fallback_read_packet(nabto_connect* con);
void unabto_tcp_fallback_handle_write(nabto_connect* con);

void close_tcp_socket(nabto_connect* con);

void unabto_tcp_fallback_select_add_to_read_fd_set(fd_set* readFds, int* maxReadFd) {
    int i;
    for (i = 0; i < NABTO_CONNECTIONS_SIZE; i++) {
        nabto_connect* con = &connections[i];

        if (con->state != CS_IDLE) {
            unabto_tcp_fallback_state st = con->tcpFallbackConnectionState;
            unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
            if (st > UTFS_CONNECTING && st < UTFS_CLOSED && fbConn->socket != INVALID_SOCKET) {
                FD_SET(fbConn->socket, readFds);
                *maxReadFd = MAX(*maxReadFd ,fbConn->socket);
            }
        }
    }
}

void unabto_tcp_fallback_select_add_to_write_fd_set(fd_set* writeFds, int* maxWriteFd) {
    int i;
    for (i = 0; i < NABTO_CONNECTIONS_SIZE; i++) {
        nabto_connect* con = &connections[i];

        if (con->state != CS_IDLE) {
            unabto_tcp_fallback_state st = con->tcpFallbackConnectionState;
            unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
            if (st > UTFS_IDLE && st < UTFS_CLOSED) {
                if ((st == UTFS_CONNECTING || fbConn->sendBufferLength > 0) && fbConn->socket != INVALID_SOCKET) {
                    FD_SET(fbConn->socket, writeFds);
                    *maxWriteFd = MAX(*maxWriteFd ,fbConn->socket);
                }
            }
        }
    }
}

void unabto_tcp_fallback_select_read_sockets(fd_set* readFds) {
    int i;
    for (i = 0; i < NABTO_CONNECTIONS_SIZE; i++) {
        nabto_connect* con = &connections[i];

        if (con->state != CS_IDLE) {
            unabto_tcp_fallback_state st = con->tcpFallbackConnectionState;
            unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
            if (st > UTFS_IDLE && st < UTFS_CLOSED && fbConn->socket != INVALID_SOCKET) {
                if (FD_ISSET(fbConn->socket, readFds)) {
                    unabto_tcp_fallback_read_packet(con);
                }
            }
        }
    }
}

void unabto_tcp_fallback_select_write_sockets(fd_set* writeFds) {
    int i;
    for (i = 0; i < NABTO_CONNECTIONS_SIZE; i++) {
        nabto_connect* con = &connections[i];

        if (con->state != CS_IDLE) {
            unabto_tcp_fallback_state st = con->tcpFallbackConnectionState;
            unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
            if (st > UTFS_IDLE && st < UTFS_CLOSED && fbConn->socket != INVALID_SOCKET) {
                if (FD_ISSET(fbConn->socket, writeFds)) {
                    if (st == UTFS_CONNECTING) {
                        unabto_tcp_fallback_handle_connect(con);
                    } else if (st >= UTFS_CONNECTED) {
                        unabto_tcp_fallback_handle_write(con);
                    }
                } 
            }
        }
    }
}

bool unabto_tcp_fallback_init(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
    memset(fbConn, 0, sizeof(unabto_tcp_fallback_connection));
    fbConn->sendBufferLength = 0;
    fbConn->socket = INVALID_SOCKET;
    return true;
}

/**
 * This is called by unabto when the resource is released. All
 * resources must have been released when returning.
 */
bool unabto_tcp_fallback_close(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];

    if (con->tcpFallbackConnectionState != UTFS_CLOSED) {
        close_tcp_socket(con);
    }
    return true;
}

void unabto_tcp_fallback_read_packet(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
    while(true) {
        if (fbConn->recvBufferLength < 16) {
            int status = recv(fbConn->socket, fbConn->recvBuffer + fbConn->recvBufferLength, 16-fbConn->recvBufferLength, 0);
            int err = errno;
            if (status < 0) {
                if ((err == EAGAIN) || err == EWOULDBLOCK) {
                    return;
                } else {
                    NABTO_LOG_ERROR((PRI_tcp_fb "unabto_tcp_fallback_read_single_packet failed", TCP_FB_ARGS(con)));
                    unabto_tcp_fallback_close(con);
                    return;
                }
            } else if (status == 0) {
                NABTO_LOG_INFO((PRI_tcp_fb "TCP fallback connection closed by peer", TCP_FB_ARGS(con)));
                unabto_tcp_fallback_close(con);
                return;
            } else {
                fbConn->recvBufferLength+=status;
            }
        }

        if (fbConn->recvBufferLength >= 16) {
            uint16_t packetLength;
            int status;
            int err;
            READ_U16(packetLength, fbConn->recvBuffer+14);
            
            status = recv(fbConn->socket, fbConn->recvBuffer + fbConn->recvBufferLength, packetLength - fbConn->recvBufferLength, 0);
            err = errno;
            if (status < 0) {
                if ((err == EAGAIN) || err == EWOULDBLOCK) {
                    return;
                } else {
                    NABTO_LOG_ERROR((PRI_tcp_fb "Tcp read failed", TCP_FB_ARGS(con)));
                    unabto_tcp_fallback_close(con);
                    return;
                }
            } else if (status == 0) {
                NABTO_LOG_INFO((PRI_tcp_fb "TCP fallback connection closed by peer", TCP_FB_ARGS(con)));
                unabto_tcp_fallback_close(con);
                return;
            } else {
                fbConn->recvBufferLength += status;
            }
            
            if (fbConn->recvBufferLength == packetLength) {
                message_event event;
                event.type = MT_TCP_FALLBACK;
                
                memcpy(nabtoCommunicationBuffer, fbConn->recvBuffer, fbConn->recvBufferLength);
                
                NABTO_LOG_TRACE((PRI_tcp_fb "Received fallback packet length %" PRIsize, TCP_FB_ARGS(con), fbConn->recvBufferLength));
                
                nabto_message_event(&event, fbConn->recvBufferLength);
                NABTO_LOG_TRACE((PRI_tcp_fb "fallback packet done\n==================================================", TCP_FB_ARGS(con)));
                
                fbConn->recvBufferLength = 0;
            }
        }
    }
}


bool unabto_tcp_fallback_connect(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
    int status;
    int flags;

    fbConn->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (fbConn->socket < 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "Could not create socket for tcp fallback.", TCP_FB_ARGS(con)));
        return false;
    }

    flags = 1;
    if (setsockopt(fbConn->socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flags, sizeof(int)) != 0) {
        NABTO_LOG_ERROR(("Could not set socket option TCP_NODELAY"));
    }




#ifndef WIN32
    flags = fcntl(fbConn->socket, F_GETFL, 0);
    if (flags < 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "fcntl fail", TCP_FB_ARGS(con))); 
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        con->tcpFallbackConnectionState = UTFS_CLOSED;
        return false;
    }
    if (fcntl(fbConn->socket, F_SETFL, flags | O_NONBLOCK) < 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "fcntl fail", TCP_FB_ARGS(con)));        
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        con->tcpFallbackConnectionState = UTFS_CLOSED;
        return false;
    }

    flags = 1;
    if(setsockopt(fbConn->socket, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags)) < 0) {
        NABTO_LOG_ERROR(("could not enable KEEPALIVE"));
    }
#ifndef __MACH__
    flags = 9;
    if(setsockopt(fbConn->socket, SOL_TCP, TCP_KEEPCNT, &flags, sizeof(flags)) < 0) {
        NABTO_LOG_ERROR(("could not set TCP_KEEPCNT"));
    }

    flags = 60;
    if(setsockopt(fbConn->socket, SOL_TCP, TCP_KEEPIDLE, &flags, sizeof(flags)) < 0) {
        NABTO_LOG_ERROR(("could not set TCP_KEEPIDLE"));
    }
    
    flags = 60;
    if(setsockopt(fbConn->socket, SOL_TCP, TCP_KEEPINTVL, &flags, sizeof(flags)) < 0) {
        NABTO_LOG_ERROR(("could not set TCP KEEPINTVL"));
    }
#else
    flags = 60;
    if(setsockopt(fbConn->socket, IPPROTO_TCP, TCP_KEEPALIVE, &flags, sizeof(flags)) < 0) {
        NABTO_LOG_ERROR(("could not set TCP_KEEPCNT"));
    }
#endif
    
#endif

    memset(&fbConn->fbHost,0,sizeof(struct sockaddr_in));
    fbConn->fbHost.sin_family = AF_INET;
    fbConn->fbHost.sin_addr.s_addr = htonl(con->fallbackHost.addr);
    fbConn->fbHost.sin_port = htons(con->fallbackHost.port);
    
    NABTO_LOG_INFO((PRI_tcp_fb "Ep. " PRIep, TCP_FB_ARGS(con), MAKE_EP_PRINTABLE(con->fallbackHost)));


    status = connect(fbConn->socket, (struct sockaddr*)&fbConn->fbHost, sizeof(struct sockaddr_in));
   
    if (status == 0) {
        con->tcpFallbackConnectionState = UTFS_CONNECTED;
    } else {
        int err = errno;
        // err is two on windows.
        if (err == EINPROGRESS) {
            con->tcpFallbackConnectionState = UTFS_CONNECTING;
        } else {
            NABTO_LOG_ERROR((PRI_tcp_fb "Could not connect to fallback tcp endpoint. %s", TCP_FB_ARGS(con), strerror(errno)));
            closesocket(fbConn->socket);
            fbConn->socket = INVALID_SOCKET;
            con->tcpFallbackConnectionState = UTFS_CLOSED;    
            return false;
        }
    }

#ifdef WIN32
    flags = 1;
    if (ioctlsocket(fbConn->socket, FIONBIO, &flags) != 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "ioctlsocket fail", TCP_FB_ARGS(con)));        
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        con->tcpFallbackConnectionState = UTFS_CLOSED;
        return false;
    }
#endif

    return true;
}

void unabto_tcp_fallback_handle_connect(nabto_connect* con) {
#ifndef WIN32
    
    int err;
    optlen len;
    unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
    len = sizeof(err);
    if (getsockopt(fbConn->socket, SOL_SOCKET, SO_ERROR, &err, &len) != 0) {
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        con->tcpFallbackConnectionState = UTFS_CLOSED;
    } else {
        if (err == 0) {
            con->tcpFallbackConnectionState = UTFS_CONNECTED;
        } else {
            closesocket(fbConn->socket);
            fbConn->socket = INVALID_SOCKET;
            con->tcpFallbackConnectionState = UTFS_CLOSED;
        }
    }
#endif
}


void close_tcp_socket(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
    closesocket(fbConn->socket);
    fbConn->socket = INVALID_SOCKET;
    con->tcpFallbackConnectionState = UTFS_CLOSED;
    unabto_tcp_fallback_socket_closed(con);
}

void unabto_tcp_fallback_handle_write(nabto_connect* con) {
	ssize_t status;
    int dataToSend;
    unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
    UNABTO_ASSERT(fbConn->sendBufferSent <= fbConn->sendBufferLength); 
    dataToSend = fbConn->sendBufferLength - fbConn->sendBufferSent;
    
    status = send(fbConn->socket, fbConn->sendBuffer + fbConn->sendBufferSent, dataToSend, MSG_NOSIGNAL);
    if (status > 0) {
        fbConn->sendBufferSent += status;
    } else if (status < 0) {
        int err = errno;
        if ((err == EAGAIN) || err == EWOULDBLOCK) {
        } else {
            NABTO_LOG_ERROR((PRI_tcp_fb "Send of tcp packet failed", TCP_FB_ARGS(con)));
            close_tcp_socket(con);
            return; 
        }
    }

    if (fbConn->sendBufferSent > fbConn->sendBufferLength) {
        NABTO_LOG_FATAL(("fbConn->sendBufferSent(%" PRIsize ") > fbConn->sendBufferLength(%" PRIsize "), that should not be possible", fbConn->sendBufferSent, fbConn->sendBufferLength));
    }
    
    if (fbConn->sendBufferSent == fbConn->sendBufferLength) {
        fbConn->sendBufferLength = 0;
    }
}


unabto_tcp_fallback_error unabto_tcp_fallback_write(nabto_connect* con, uint8_t* buffer, size_t bufferLength) {

    unabto_tcp_fallback_connection* fbConn = &fbConns[nabto_connection_index(con)];
    unabto_tcp_fallback_error status = UTFE_OK;

    if (fbConn->sendBufferLength == 0) {
        memcpy(fbConn->sendBuffer, buffer, bufferLength);
        fbConn->sendBufferLength = bufferLength;
        fbConn->sendBufferSent = 0;
        unabto_tcp_fallback_handle_write(con);
        status = UTFE_OK;
        NABTO_LOG_TRACE((PRI_tcp_fb "Succesful queue of tcp fallback packet.", TCP_FB_ARGS(con)));
    } else {
        NABTO_LOG_TRACE((PRI_tcp_fb "Could not enqueue tcp fallback packet.", TCP_FB_ARGS(con)));
        status = UTFE_QUEUE_FULL;
    }

    return status;
}
