/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifndef _UNABTO_PACKET_UTIL_H_
#define _UNABTO_PACKET_UTIL_H_
/**
 * @file
 * Nabto uServer packet utilities - Interface.
 *
 */

#include <unabto/unabto_protocol_defines.h>
#include <unabto/unabto_environment.h>

#if NABTO_ENABLE_CONNECTIONS

#ifdef __cplusplus
extern "C" {
#endif

/** Nabto Packet Header. */
typedef struct {
    uint32_t nsi_cp;    /**< NSI.cp      */
    uint32_t nsi_sp;    /**< NSI.sp      */
    uint8_t  type;      /**< type        */
    uint8_t  version;   /**< version     */
    uint8_t  rsvd;      /**< reserved    */
    uint8_t  flags;     /**< flags       */
    uint16_t seq;       /**< sequence    */
    uint16_t len;       /**< length      */
    uint8_t  nsi_co[8]; /**< NSI.co(opt.)*/
    uint16_t tag;       /**< tag(opt.)   */

    uint16_t hlen;      /**< header length */
} nabto_packet_header;

/** Packet and payload size and offset declarations. */
enum {
    SIZE_HEADER         = NP_PACKET_HDR_MIN_BYTELENGTH, ///< the size of the fixed size header
    SIZE_HEADER_MAX     = NP_PACKET_HDR_MAX_BYTELENGTH, ///< the max size of a header (fixed + nsi.co + tag)
    SIZE_PAYLOAD_HEADER = NP_PAYLOAD_HDR_BYTELENGTH,    ///< the size of a payload header
    SIZE_CODE           = 2,          ///< the size of the crypto payload code field

    /* from start of buffer/header: */
    OFS_PACKET_FLAGS    = 11,
    OFS_PACKET_LGT      = SIZE_HEADER - 2,

    /* from start of payload: */
    OFS_PAYLOAD_LGT     = 2,
    OFS_CODE            = SIZE_PAYLOAD_HEADER,
    OFS_DATA            = SIZE_PAYLOAD_HEADER + SIZE_CODE
};


/** Packet Header Types, copied from protocol/header.hpp. */
typedef enum {
    DATA       = NP_PACKET_HDR_TYPE_DATA,
    GW_CONN    = NP_PACKET_HDR_TYPE_GW_CONN,
    GW_CONN_U  = NP_PACKET_HDR_TYPE_GW_CONN_U,

    U_LIMIT    = NP_PACKET_HDR_TYPE_U_LIMIT,
    U_INVITE   = NP_PACKET_HDR_TYPE_U_INVITE,
    U_ATTACH   = NP_PACKET_HDR_TYPE_U_ATTACH,
    U_ALIVE    = NP_PACKET_HDR_TYPE_U_ALIVE,
    U_CONNECT  = NP_PACKET_HDR_TYPE_U_CONNECT,
    U_DEBUG    = NP_PACKET_HDR_TYPE_U_DEBUG
} nabto_header_type;


/** The Nabto Payload Types */
typedef enum np_payload_type_e type;

/** Crypto suite codes. Copied from cryptocontext.hpp */
typedef enum {
    CRYPT_W_NULL_DATA           = NP_PAYLOAD_CRYPTO_OPER_CRYPT |
                                  NP_PAYLOAD_CRYPTO_ENCR_SECR,
    CRYPT_W_AES_CBC_HMAC_SHA256 = NP_PAYLOAD_CRYPTO_OPER_CRYPT |
                                  NP_PAYLOAD_CRYPTO_ENCR_SECR |
                                  NP_PAYLOAD_CRYPTO_SYMM_AES_CBC |
                                  NP_PAYLOAD_CRYPTO_HASH_HMAC_SHA256
} crypto_suite;


/** Capability bit masks. Equivalent to CAP_B_* in peercapability.hpp */
enum {

// avoid annoying "shift expression has no effect" warning on PIC18
#if NP_PAYLOAD_CAPA_BIT_CAP_DIR == 0
    PEER_CAP_DIR      = 1,                                      ///< See #NP_PAYLOAD_CAPA_BIT_CAP_DIR
#else
    PEER_CAP_DIR      = 1 << NP_PAYLOAD_CAPA_BIT_CAP_DIR,       ///< See #NP_PAYLOAD_CAPA_BIT_CAP_DIR
#endif

    PEER_CAP_FB_TCP   = 1 << NP_PAYLOAD_CAPA_BIT_CAP_FB_TCP,    ///< See #NP_PAYLOAD_CAPA_BIT_CAP_FB_TCP
    PEER_CAP_ENCR_OFF = 1 << NP_PAYLOAD_CAPA_BIT_CAP_ENCR_OFF,  ///< See #NP_PAYLOAD_CAPA_BIT_CAP_ENCR_OFF
    PEER_CAP_MICRO    = 1 << NP_PAYLOAD_CAPA_BIT_CAP_MICRO,     ///< See #NP_PAYLOAD_CAPA_BIT_CAP_MICRO
    PEER_CAP_UDP      = 1 << NP_PAYLOAD_CAPA_BIT_CAP_UDP,       ///< See #NP_PAYLOAD_CAPA_BIT_CAP_UDP
    PEER_CAP_TAG      = 1 << NP_PAYLOAD_CAPA_BIT_CAP_TAG,       ///< See #NP_PAYLOAD_CAPA_BIT_CAP_TAG
    PEER_CAP_FRCTRL   = 1 << NP_PAYLOAD_CAPA_BIT_CAP_FRCTRL,    ///< See #NP_PAYLOAD_CAPA_BIT_CAP_FRCTRL
    PEER_CAP_PROXY    = 1 << NP_PAYLOAD_CAPA_BIT_CAP_PROXY,     ///< See #NP_PAYLOAD_CAPA_BIT_CAP_PROXY
    PEER_CAP_ASYNC    = 1 << NP_PAYLOAD_CAPA_BIT_CAP_ASYNC,     ///< See #NP_PAYLOAD_CAPA_BIT_CAP_ASYNC
    PEER_CAP_FB_TCP_U = 1 << NP_PAYLOAD_CAPA_BIT_CAP_FB_TCP_U   ///< See #NP_PAYLOAD_CAPA_BIT_CAP_FB_TCP_U
};


/** same as Notification::NOTIFY_****, don't change values, used in protocol */
enum {
    NOTIFY_ATTACH_OK  = 1,                ///< The SP_ATTACH procedure succeeded.
    NOTIFY_CONNECT_OK = NOTIFY_ATTACH_OK, ///< The Connect procedure succeeded (same value as NOTIFY_ATTACH_OK)
    NOTIFY_ATTACH_CLOSE,                  ///< The SP_ATTACH closedown notification.
    NOTIFY_MICRO_ACK,                     ///< The Micro has received the request, but the answer isn't ready yet

    //Protocol error codes (0x8000-0xFFFF)
    NOTIFY_ERROR                   = NP_PAYLOAD_NOTIFY_ERROR,                   ///< See #NP_PAYLOAD_NOTIFY_ERROR
    NOTIFY_ERROR_UNKNOWN_SERVER    = NP_PAYLOAD_NOTIFY_ERROR_UNKNOWN_SERVER,    ///< See #NP_PAYLOAD_NOTIFY_ERROR_UNKNOWN_SERVER
    NOTIFY_ERROR_SP_CERTIFICATE    = NP_PAYLOAD_NOTIFY_ERROR_SP_CERTIFICATE,    ///< See #NP_PAYLOAD_NOTIFY_ERROR_SP_CERTIFICATE
    NOTIFY_ERROR_CP_CERTIFICATE    = NP_PAYLOAD_NOTIFY_ERROR_CP_CERTIFICATE,    ///< See #NP_PAYLOAD_NOTIFY_ERROR_CP_CERTIFICATE
    NOTIFY_ERROR_CP_ACCESS         = NP_PAYLOAD_NOTIFY_ERROR_CP_ACCESS,         ///< See #NP_PAYLOAD_NOTIFY_ERROR_CP_ACCESS
    NOTIFY_ERROR_PROTOCOL_VERSION  = NP_PAYLOAD_NOTIFY_ERROR_PROTOCOL_VERSION,  ///< See #NP_PAYLOAD_NOTIFY_ERROR_PROTOCOL_VERSION
    NOTIFY_ERROR_BAD_CERT_ID       = NP_PAYLOAD_NOTIFY_ERROR_BAD_CERT_ID,       ///< See #NP_PAYLOAD_NOTIFY_ERROR_BAD_CERT_ID
    NOTIFY_ERROR_UNKNOWN_MICRO     = NP_PAYLOAD_NOTIFY_ERROR_UNKNOWN_MICRO,     ///< See #NP_PAYLOAD_NOTIFY_ERROR_UNKNOWN_MICRO
    NOTIFY_ERROR_ENCR_MISMATCH     = NP_PAYLOAD_NOTIFY_ERROR_ENCR_MISMATCH,     ///< See #NP_PAYLOAD_NOTIFY_ERROR_ENCR_MISMATCH
    NOTIFY_ERROR_BUSY_MICRO        = NP_PAYLOAD_NOTIFY_ERROR_BUSY_MICRO,        ///< See #NP_PAYLOAD_NOTIFY_ERROR_BUSY_MICRO
    NOTIFY_ERROR_MICRO_REQ_ERR     = NP_PAYLOAD_NOTIFY_ERROR_MICRO_REQ_ERR,     ///< See #NP_PAYLOAD_NOTIFY_ERROR_MICRO_REQ_ERR
    NOTIFY_ERROR_MICRO_REATTACHING = NP_PAYLOAD_NOTIFY_ERROR_MICRO_REATTACHING, ///< See #NP_PAYLOAD_NOTIFY_ERROR_MICRO_REATTACHING
    NOTIFY_LAST_ERROR 
};


/**
 * Read a packet header
 * @param buf   the start of the inputbuffer
 * @param end   the end of the inputbuffer
 * @param hdr   the header
 * @return      the size of the header (0 when error)
 */
uint16_t nabto_rd_header(const uint8_t* buf, const uint8_t* end, nabto_packet_header* hdr);


/**
 * Read a payload header
 * @param buf   the start of the inputbuffer
 * @param end   the end of the inputbuffer
 * @param type  type of the received payload
 * @return      the size of the payload (excl payload header)
 *              return 0 means error.
 */
uint16_t nabto_rd_payload(const uint8_t* buf, const uint8_t* end, uint8_t* type);

/**
 * Find a payload type
 * @param buf   The start of the block of payloads. 
 * @param end   the end of the input
 * @param type  the payload type to find.
 * @param payloadStart   the start of the found payload INCLUDING THE HEADER BYTES
 * @param payloadLength  the length of the found payload MINUS THE PAYLOAD_HDR_BYTELENGTH.
 * @return true if a payload with the given type was found.
 */
bool find_payload(uint8_t* buf, uint8_t* end, uint8_t type, uint8_t** payloadStart, uint16_t* payloadLength);

/**
 * Write the packet header (excl the length field)
 * @param buf    the start of the packet
 * @param cpnsi  the stream ID, cp part
 * @param spnsi  the stream ID, sp part
 * @param type   the packet type
 * @param rsp    response flag
 * @param seq    the sequence number
 * @param tag    tag
 * @return       the first byte after the header
 */
uint8_t* insert_header(uint8_t* buf, uint32_t cpnsi, uint32_t spnsi, uint8_t type, bool rsp, uint16_t seq, uint16_t tag, uint8_t* nsico);


/**
 * Write the packet header of a DATA packet (excl the length field)
 * @param buf    the start of the packet
 * @param nsi    the stream ID
 * @param nsico  the nsi.co to be inserted or zero to exclude it from the header
 * @param tag    tag
 * @return       the first byte after the header
 */
uint8_t* insert_data_header(uint8_t* buf, uint32_t nsi, uint8_t* nsico, uint16_t tag);

/**
 * Write packet flags.
 * @param buf    (uint8_t*) the start of the packet
 * @param flags  (uint8_t) the flags
 */
#define insert_flags(buf, flags)    WRITE_U8((uint8_t*)(buf) + OFS_PACKET_FLAGS, flags)

/**
 * Add packet flags.
 * @param buf    (uint8_t*) the start of the packet
 * @param flags  (uint8_t) the flag bits to be added
 */
#define add_flags(buf, flags)    do { *((uint8_t*)(buf) + OFS_PACKET_FLAGS) |= flags; } while (0)

/**
 * Write packet length.
 * @param buf  (uint8_t*) the start of the packet
 * @param len  (size_t) the total length of the payload
 */
#define insert_length(buf, len)     WRITE_U16((uint8_t*)(buf) + OFS_PACKET_LGT, (uint16_t)(len))

/**
 * Write the Payload
 * @param buf      the place to put the payload
 * @param type     the payload type
 * @param content  the content of the payload (may be 0)
 * @param size     the size of the payload (excl payload header)
 * @return         the first byte after the payload
 */
uint8_t* insert_payload(uint8_t* buf, uint8_t type, const uint8_t* content, size_t size);

/**
 * Write the Payload with the OPTIONAL flag set
 * @param buf      the place to put the payload
 * @param type     the payload type
 * @param content  the content of the payload (may be 0)
 * @param size     the size of the payload (excl payload header)
 * @return         the first byte after the payload
 */
uint8_t* insert_optional_payload(uint8_t* buf, uint8_t type, const uint8_t* content, size_t size);

/** 
 * insert a capabilities packet
 * @param buf           the buffer
 * @param cap_encr_off  true if we accept unencrypted connections
 * @return pointer to end of packet.
 */
uint8_t* insert_capabilities(uint8_t* buf, bool cap_encr_off);

/**
 * insert payloads, return NULL if there is not enough room for the payload in the buffer
 */
uint8_t* insert_ipx_payload(uint8_t* ptr, uint8_t* end);
uint8_t* insert_version_payload(uint8_t* ptr, uint8_t* end);
uint8_t* insert_sp_id_payload(uint8_t* ptr, uint8_t* end);
uint8_t* insert_stats_payload(uint8_t* ptr, uint8_t* end, uint8_t stats_event_type);
uint8_t* insert_notify_payload(uint8_t* buf, uint8_t* end, uint32_t notifyValue);
uint8_t* insert_piggy_payload(uint8_t* ptr, uint8_t* end, uint8_t* piggyData, uint16_t piggySize);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* NABTO_ENABLE_CONNECTIONS */

#endif
