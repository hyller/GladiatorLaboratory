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
#include <modules/tcp_fallback/tcp_fallback.h>
#include <unabto/unabto_util.h>
#include <errno.h>
#include <unabto_api/unabto_api_internal.h>
#include <unabto/unabto_memory.h>
#include <unabto/unabto_message.h>
#include <unabto/unabto_tcp_fallback.h>

#ifdef WIN32
#define SHUT_RDWR SD_BOTH
#define usleep(t) Sleep((t)/1000)
#define write(s,d,l) _write(s,d,l)
#else
#define INVALID_SOCKET (-1)
#define closesocket(s) close(s)
#endif

static bool unabto_tcp_fallback_read_single_packet(nabto_connect* con, uint8_t* receiveBuffer, size_t receiveBufferLength, size_t* receiveLength);
static bool unabto_tcp_fallback_send_full_packet(nabto_connect* con, uint8_t* data, size_t dataLength);

static void wait_until_false(unabto_mutex_t* mutex, unabto_cond_t* condition, bool* flag);
static void set_flag(unabto_mutex_t* mutex, bool* flag, bool value);
static void tcp_fallback_connection_lost(nabto_connect* con);


bool unabto_tcp_fallback_init(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &con->fbConn;
    unabto_mutex_init(&fbConn->sendBufferMutex);
    unabto_mutex_init(&fbConn->recvRunningMutex);
    unabto_mutex_init(&fbConn->sendRunningMutex);
    unabto_cond_init(&fbConn->sendBufferCondition);
    unabto_cond_init(&fbConn->recvRunningCondition);
    unabto_cond_init(&fbConn->sendRunningCondition);
    fbConn->sendBufferLength = 0;
    fbConn->recvRunning = false;
    fbConn->sendRunning = false;
    fbConn->closed = false;
    fbConn->socket = INVALID_SOCKET;
#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
    return true;
}

void wait_until_false(unabto_mutex_t* mutex, unabto_cond_t* condition, bool* flag) {
    unabto_mutex_lock(mutex);
    while (*flag) {
        unabto_cond_wait(condition, mutex);
    }
    unabto_mutex_unlock(mutex);
}

void set_flag(unabto_mutex_t* mutex, bool* flag, bool value) {
    unabto_mutex_lock(mutex);
    *flag = value;
    unabto_mutex_unlock(mutex);
}

/**
 * This is called by unabto when the resource is released. All
 * resources must have been released when returning.
 */
bool unabto_tcp_fallback_close(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &con->fbConn;
    if (fbConn->closed) {
        NABTO_LOG_TRACE((PRI_tcp_fb "already closed", TCP_FB_ARGS(con)));
        return true;
    }
    fbConn->closed = true;
    shutdown(fbConn->socket, SHUT_RDWR);

    unabto_mutex_lock(&fbConn->sendBufferMutex);
    fbConn->sendBufferLength = 0; // XXX prevent calls to wait in fallback_sender after this point
    unabto_cond_broadcast(&fbConn->sendBufferCondition);
    unabto_mutex_unlock(&fbConn->sendBufferMutex);

    NABTO_LOG_TRACE((PRI_tcp_fb "Waiting for sender thread", TCP_FB_ARGS(con)));
    wait_until_false(&fbConn->sendRunningMutex, &fbConn->sendRunningCondition, &fbConn->sendRunning);
    NABTO_LOG_TRACE((PRI_tcp_fb "Waiting for receiver thread", TCP_FB_ARGS(con)));
    wait_until_false(&fbConn->recvRunningMutex, &fbConn->recvRunningCondition, &fbConn->recvRunning);
    NABTO_LOG_TRACE((PRI_tcp_fb "Done waiting for threads", TCP_FB_ARGS(con)));

    unabto_tcp_fallback_socket_closed(con);

    closesocket(fbConn->socket);
    fbConn->socket = INVALID_SOCKET;

    unabto_mutex_destroy(&fbConn->sendBufferMutex);
    unabto_cond_destroy(&fbConn->sendBufferCondition);

    unabto_mutex_destroy(&fbConn->sendRunningMutex);
    unabto_cond_destroy(&fbConn->sendRunningCondition);

    unabto_mutex_destroy(&fbConn->recvRunningMutex);
    unabto_cond_destroy(&fbConn->recvRunningCondition);

    return true;
}

bool unabto_tcp_fallback_send_full_packet(nabto_connect* con, uint8_t* data, size_t dataLength) {
    while (dataLength > 0 && !con->fbConn.closed) {
        ssize_t status = write(con->fbConn.socket, data, dataLength);
        int err = errno;
        if (status > 0) {
            dataLength -= status;
            data += status;
        } else if (status == 0) {
            usleep(1000);
        } else {
            if ((err == EAGAIN) || err == EWOULDBLOCK) {
                usleep(1000);
            } else {
                NABTO_LOG_ERROR((PRI_tcp_fb "Send of tcp packet failed", TCP_FB_ARGS(con)));
                return false;
            }
        }
    }
    if (con->fbConn.closed) {
        return false;
    } else {
        return true;
    }
}

/**
 * receive a full nabto frame into the buffer
 */
static intptr_t unabto_tcp_fallback_receiver(void* args) {
    nabto_connect* con = (nabto_connect*)args;
    unabto_tcp_fallback_connection* fbConn = &con->fbConn;
    uint8_t buffer[65536];
    size_t packetLength;

    set_flag(&fbConn->recvRunningMutex, &fbConn->recvRunning, true);

    while (!fbConn->closed) {
        if (!unabto_tcp_fallback_read_single_packet(con, buffer, 65536,&packetLength)) {
            NABTO_LOG_TRACE((PRI_tcp_fb "Could not read framed nabto packet from tcp stream", TCP_FB_ARGS(con)));
            tcp_fallback_connection_lost(con);
            break;
        }
        
        unabto_api_lock_take();

        memcpy(nabtoCommunicationBuffer, buffer, packetLength);

        NABTO_LOG_TRACE((PRI_tcp_fb "Received fallback packet length %" PRIsize, TCP_FB_ARGS(con), packetLength));
        
        message_event event;
        event.type = MT_TCP_FALLBACK;

        nabto_message_event(&event, packetLength);
        NABTO_LOG_TRACE((PRI_tcp_fb "fallback packet done\n==================================================", TCP_FB_ARGS(con)));

        unabto_api_lock_release();
//        unabto_api_yield_notify();
    }
    
    set_flag(&fbConn->recvRunningMutex, &fbConn->recvRunning, false);
    unabto_cond_broadcast(&fbConn->recvRunningCondition);

    return 0;
}

bool unabto_tcp_fallback_read_single_packet(nabto_connect* con, uint8_t* receiveBuffer, size_t receiveBufferLength, size_t* receiveLength) {
    size_t readLength = 0;
    uint8_t* receivePtr = receiveBuffer;
    uint16_t packetLength;
    
    // read the header
    while (readLength < 16 && !con->fbConn.closed) {
        
        int status = recv(con->fbConn.socket, receivePtr, 16-readLength, 0);
        int err = errno;
        if (status < 0) {
            if ((err == EAGAIN) || err == EWOULDBLOCK) {
                usleep(1000);
            } else {
                NABTO_LOG_ERROR((PRI_tcp_fb "unabto_tcp_fallback_read_single_packet failed", TCP_FB_ARGS(con)));
                return false;
            }
        } else if (status == 0) {
            NABTO_LOG_INFO((PRI_tcp_fb "TCP fallback connection closed by peer", TCP_FB_ARGS(con)));
            return false;
        } else {
            readLength += status;
            receivePtr += status;
        }
    }
    
    if (con->fbConn.closed) {
        NABTO_LOG_TRACE((PRI_tcp_fb "connection closed after %u bytes", TCP_FB_ARGS(con), (unsigned int) readLength));
        return false;
    }

    if (readLength != 16) {
        NABTO_LOG_FATAL((PRI_tcp_fb "readLength is not 16 which it should be", TCP_FB_ARGS(con)));
    }
    
    READ_U16(packetLength, receivePtr-2);

    while (readLength < packetLength && !con->fbConn.closed) {
        int status = recv(con->fbConn.socket, receivePtr, packetLength - readLength, 0);
        int err = errno;
        if (status < 0) {
            if ((err == EAGAIN) || err == EWOULDBLOCK) {
                usleep(1000);
            } else {
                NABTO_LOG_ERROR((PRI_tcp_fb "Tcp read failed", TCP_FB_ARGS(con)));
                return false;
            }
        } else if (status == 0) {
            NABTO_LOG_INFO((PRI_tcp_fb "TCP fallback connection closed by peer", TCP_FB_ARGS(con)));
            return false;
        } else {
            readLength += status;
            receivePtr += status;
        }
    }
    
    if (con->fbConn.closed) {
        return false;
    } else {
        *receiveLength = readLength;
        return true;
    }
}
 
/**
 * take a message from the queue, send it!
 */
static intptr_t unabto_tcp_fallback_sender(void* data) {
    nabto_connect* con = (nabto_connect*)data;
    unabto_tcp_fallback_connection* fbConn = &con->fbConn;

    set_flag(&fbConn->sendRunningMutex, &fbConn->sendRunning, true);

    while (!fbConn->closed) {
        unabto_mutex_lock(&fbConn->sendBufferMutex);
        if (fbConn->sendBufferLength == 0) {
            unabto_cond_wait(&fbConn->sendBufferCondition, &fbConn->sendBufferMutex);
            unabto_mutex_unlock(&fbConn->sendBufferMutex);
        } else {
            uint8_t* data;
            size_t dataLength;
            unabto_mutex_unlock(&fbConn->sendBufferMutex);
            if (fbConn->closed) {
                NABTO_LOG_TRACE((PRI_tcp_fb "Closed while waiting", TCP_FB_ARGS(con)));
                break;
            }
            data = fbConn->sendBuffer;
            dataLength = fbConn->sendBufferLength;

            if (!unabto_tcp_fallback_send_full_packet(con, data, dataLength)) {
                NABTO_LOG_ERROR((PRI_tcp_fb "Could not send the full tcp fallback packet, closing the tcp sender", TCP_FB_ARGS(con)));
                tcp_fallback_connection_lost(con);
                break;
            }
            unabto_mutex_lock(&fbConn->sendBufferMutex);
            fbConn->sendBufferLength = 0;
            unabto_mutex_unlock(&fbConn->sendBufferMutex);
        }
    }
    
    set_flag(&fbConn->sendRunningMutex, &fbConn->sendRunning, false);
    unabto_cond_broadcast(&fbConn->sendRunningCondition);

    return 0;
}

bool unabto_tcp_fallback_connect(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &con->fbConn;
    struct sockaddr_in fbHost;
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

    memset(&fbHost,0,sizeof(fbHost));
    fbHost.sin_family = AF_INET;
    fbHost.sin_addr.s_addr = htonl(con->fallbackHost.addr);
    fbHost.sin_port = htons(con->fallbackHost.port);
    
    NABTO_LOG_INFO((PRI_tcp_fb "Ep. " PRIep, TCP_FB_ARGS(con), MAKE_EP_PRINTABLE(con->fallbackHost)));

    status = connect(fbConn->socket, (struct sockaddr*)&fbHost, sizeof(fbHost));
    if (status < 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "Could not connect to fallback tcp endpoint.", TCP_FB_ARGS(con)));
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        fbConn->closed = true;
        return false;
    }

    con->tcpFallbackConnectionState = UTFS_CONNECTED;

#ifdef WIN32
    flags = 1;
    if (ioctlsocket(fbConn->socket, FIONBIO, &flags) != 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "ioctlsocket fail", TCP_FB_ARGS(con)));        
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        fbConn->closed = true;
        return false;
    }
#else
    flags = fcntl(fbConn->socket, F_GETFL, 0);
    if (flags < 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "fcntl fail", TCP_FB_ARGS(con)));        
        return false;
    }
    if (fcntl(fbConn->socket, F_SETFL, flags | O_NONBLOCK) < 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "fcntl fail", TCP_FB_ARGS(con)));        
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        fbConn->closed = true;
        return false;
    }
#endif

    if (unabto_thread_create(&fbConn->receiveThread, unabto_tcp_fallback_receiver, con) != 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "Could not create receiver thread for tcp fallback.", TCP_FB_ARGS(con)));
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        fbConn->closed = true;
        return false;
    }

    if (unabto_thread_create(&fbConn->sendThread, unabto_tcp_fallback_sender, con) != 0) {
        NABTO_LOG_ERROR((PRI_tcp_fb "Could not create sender thread for tcp fallback.", TCP_FB_ARGS(con)));
        closesocket(fbConn->socket);
        fbConn->socket = INVALID_SOCKET;
        fbConn->closed = true;
        unabto_thread_detach(fbConn->receiveThread);
        return false;
    }
    unabto_thread_detach(fbConn->receiveThread);
    unabto_thread_detach(fbConn->sendThread);

    return true;
}

unabto_tcp_fallback_error unabto_tcp_fallback_write(nabto_connect* con, uint8_t* buffer, size_t bufferLength) {

    unabto_tcp_fallback_connection* fbConn = &con->fbConn;
    unabto_tcp_fallback_error status = UTFE_OK;

    unabto_mutex_lock(&fbConn->sendBufferMutex);
    if (fbConn->sendBufferLength == 0) {
        memcpy(fbConn->sendBuffer, buffer, bufferLength);
        fbConn->sendBufferLength = bufferLength;
        status = UTFE_OK;
        NABTO_LOG_TRACE((PRI_tcp_fb "Succesful queue of tcp fallback packet.", TCP_FB_ARGS(con)));
    } else {
        NABTO_LOG_TRACE((PRI_tcp_fb "Could not enqueue tcp fallback packet.", TCP_FB_ARGS(con)));
        status = UTFE_QUEUE_FULL;
    }
    if (status == UTFE_OK) {
        unabto_cond_broadcast(&fbConn->sendBufferCondition);
    }
    unabto_mutex_unlock(&fbConn->sendBufferMutex);

    return status;
}

void tcp_fallback_connection_lost(nabto_connect* con) {
    unabto_tcp_fallback_connection* fbConn = &con->fbConn;
    if (fbConn->closed) {
        NABTO_LOG_TRACE((PRI_tcp_fb "fbConn->closed true", TCP_FB_ARGS(con)));
        return;
    }
    NABTO_LOG_TRACE((PRI_tcp_fb "tcp_fallback_connection_lost", TCP_FB_ARGS(con)));
    fbConn->closed = true;
    shutdown(fbConn->socket, SHUT_RDWR);

    unabto_cond_broadcast(&fbConn->sendBufferCondition);

    unabto_api_lock_take();
    unabto_tcp_fallback_socket_closed(con);
    unabto_api_lock_release();
}
