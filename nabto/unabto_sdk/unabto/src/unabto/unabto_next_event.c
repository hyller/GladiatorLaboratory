/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#include "unabto_env_base.h"
#include "unabto_memory.h"
#include "unabto_common_main.h"
#include "unabto_stream.h"
#include "unabto_connection.h"
#include "unabto_next_event.h"
#include "unabto_stream.h"
#include "unabto_external_environment.h"

#if NABTO_ENABLE_NEXT_EVENT
#if NABTO_ENABLE_CONNECTIONS
static void nabto_connection_update_next_event(nabto_stamp_t* current_min_stamp);
#endif
#endif


#if NABTO_ENABLE_NEXT_EVENT
void unabto_next_event(nabto_stamp_t* stamp) {
    // return the smallest of context.stamp, connection.stamp, stream.stamp
    *stamp = nmc.context.timestamp;
   
#if NABTO_ENABLE_CONNECTIONS
    nabto_connection_update_next_event(stamp);
#endif
   
#if NABTO_ENABLE_STREAM
    nabto_stream_update_next_event(stamp);
#endif
}
#endif

#if NABTO_ENABLE_NEXT_EVENT
#if NABTO_ENABLE_CONNECTIONS
/**
 * update the stamp given in current_min_stamp if a connection has a
 * stamp which occurs sooner in time than the current_minimum_stamp
 */
static void nabto_connection_update_next_event(nabto_stamp_t* current_min_stamp) {
    nabto_connect* con;
    for (con = connections; con < connections + NABTO_CONNECTIONS_SIZE; con++) {
        if (con->state != CS_IDLE) {
            nabto_rendezvous_connect_state* rcs = &con->rendezvousConnectState;

            if (con->sendConnectStatistics || con->sendConnectionEndedStatistics) {
                *current_min_stamp = nabtoGetStamp();
            }

            if (nabto_connection_has_keep_alive(con)) {
                nabto_update_min_stamp(current_min_stamp, &con->stamp);
            }

            if (rcs->state == RS_CONNECTING) {
                nabto_update_min_stamp(current_min_stamp, &rcs->timestamp);

                if (rcs->openManyPorts) {
                    nabto_update_min_stamp(current_min_stamp, &rcs->openManyPortsStamp);
                }

#if NABTO_ENABLE_EXTENDED_RENDEZVOUS_MULTIPLE_SOCKETS
                if (rcs->openManySockets) {
                    nabto_update_min_stamp(current_min_stamp, &rcs->openManySocketsStamp);
                }
#endif

            }
        }
    }
}
#endif
#endif

#if NABTO_ENABLE_NEXT_EVENT
void nabto_update_min_stamp(nabto_stamp_t* current_minimum_stamp, nabto_stamp_t* stamp) {
    if (nabtoStampLess(stamp, current_minimum_stamp)) {
        *current_minimum_stamp = *stamp;
    }
}

#endif
