#include "../include/mqtt.h"
#include "../include/pack.h"
#include <stdlib.h>

/**
 * @file mqtt.c
 * @brief Implementation of MQTT protocol packet processing functions
 *
 * This file provides functions for packing and unpacking MQTT protocol packets
 * according to the MQTT v3.1.1 specification. It includes utilities for length
 * encoding/decoding and packet-specific serialization/deserialization.
 */

/** @name Private helper functions for packing and unpacking MQTT packets */
/**@{*/
/**
 * @brief Unpacks an MQTT CONNECT packet from a byte buffer
 *
 * @param[in] buf Pointer to the buffer containing the packet
 * @param[in] hdr Pointer to the parsed packet header
 * @param[out] pkt Pointer to store the unpacked packet
 * @return Size of the unpacked packet in bytes
 */
static size_t unpack_mqtt_connect(const unsigned char *, union mqtt_header *,
                                 union mqtt_packet *);

/**
 * @brief Unpacks an MQTT PUBLISH packet from a byte buffer
 *
 * @param[in] buf Pointer to the buffer containing the packet
 * @param[in] hdr Pointer to the parsed packet header
 * @param[out] pkt Pointer to store the unpacked packet
 * @return Size of the unpacked packet in bytes
 */
static size_t unpack_mqtt_publish(const unsigned char *, union mqtt_header *,
                                 union mqtt_packet *);

/**
 * @brief Unpacks an MQTT SUBSCRIBE packet from a byte buffer
 *
 * @param[in] buf Pointer to the buffer containing the packet
 * @param[in] hdr Pointer to the parsed packet header
 * @param[out] pkt Pointer to store the unpacked packet
 * @return Size of the unpacked packet in bytes
 */
static size_t unpack_mqtt_subscribe(const unsigned char *, union mqtt_header *,
                                   union mqtt_packet *);

/**
 * @brief Unpacks an MQTT UNSUBSCRIBE packet from a byte buffer
 *
 * @param[in] buf Pointer to the buffer containing the packet
 * @param[in] hdr Pointer to the parsed packet header
 * @param[out] pkt Pointer to store the unpacked packet
 * @return Size of the unpacked packet in bytes
 */
static size_t unpack_mqtt_unsubscribe(const unsigned char *,
                                     union mqtt_header *, union mqtt_packet *);

/**
 * @brief Unpacks an MQTT acknowledgment packet (PUBACK, PUBREC, etc.) from a byte buffer
 *
 * @param[in] buf Pointer to the buffer containing the packet
 * @param[in] hdr Pointer to the parsed packet header
 * @param[out] pkt Pointer to store the unpacked packet
 * @return Size of the unpacked packet in bytes
 */
static size_t unpack_mqtt_ack(const unsigned char *, union mqtt_header *,
                             union mqtt_packet *);

/**
 * @brief Packs an MQTT header into a byte buffer
 *
 * @param[in] hdr Pointer to the MQTT header to pack
 * @return Pointer to the packed header buffer (must be freed by caller)
 */
static unsigned char *pack_mqtt_header(const union mqtt_header *);

/**
 * @brief Packs an MQTT acknowledgment packet into a byte buffer
 *
 * @param[in] pkt Pointer to the packet to pack
 * @return Pointer to the packed packet buffer (must be freed by caller)
 */
static unsigned char *pack_mqtt_ack(const union mqtt_packet *);

/**
 * @brief Packs an MQTT CONNACK packet into a byte buffer
 *
 * @param[in] pkt Pointer to the packet to pack
 * @return Pointer to the packed packet buffer (must be freed by caller)
 */
static unsigned char *pack_mqtt_connack(const union mqtt_packet *);

/**
 * @brief Packs an MQTT SUBACK packet into a byte buffer
 *
 * @param[in] pkt Pointer to the packet to pack
 * @return Pointer to the packed packet buffer (must be freed by caller)
 */
static unsigned char *pack_mqtt_suback(const union mqtt_packet *);

/**
 * @brief Packs an MQTT PUBLISH packet into a byte buffer
 *
 * @param[in] pkt Pointer to the packet to pack
 * @return Pointer to the packed packet buffer (must be freed by caller)
 */
static unsigned char *pack_mqtt_publish(const union mqtt_packet *);
/**@}*/

/** @name Constants and helper functions for length encoding/decoding */
/**@{*/
/**
 * @brief Maximum number of bytes for the Remaining Length field in MQTT packet header
 *
 * According to the MQTT v3.1.1 standard, the Remaining Length field can be at
 * most 4 bytes.
 */
static const int MAX_LEN_BYTES = 4;

/**
 * @brief Encodes the Remaining Length field of an MQTT packet header
 *
 * This function implements the MQTT v3.1.1 algorithm for encoding the Remaining Length,
 * which is the length of the Variable Header and Payload combined. It does not account
 * for the bytes required to store the length itself.
 *
 * @param[out] buf Buffer to store the encoded length
 * @param[in] len Length value to encode
 * @return Number of bytes used for encoding the length
 */
int mqtt_encode_length(unsigned char *buf, size_t len)
{
    int bytes = 0;
    do {
        if (bytes + 1 > MAX_LEN_BYTES)
            return bytes;
        short d = len % 128;
        len /= 128;
        if (len > 0)
            d |= 128;
        buf[bytes++] = d;
    } while (len > 0);
    return bytes;
}

/**
 * @brief Decodes the Remaining Length field of an MQTT packet header
 *
 * This function implements the MQTT v3.1.1 algorithm for decoding the Remaining Length.
 * It advances the buffer pointer to the next byte after the length field.
 *
 * @param[in,out] buf Pointer to the buffer containing the encoded length
 * @return Decoded length value
 * @todo Handle case where multiplier > 128 * 128 * 128
 */
unsigned long long mqtt_decode_length(const unsigned char **buf)
{
    char c;
    int multiplier = 1;
    unsigned long long value = 0LL;
    do {
        c = **buf;
        value += (c & 127) * multiplier;
        multiplier *= 128;
        (*buf)++;
    } while ((c & 128) != 0);
    return value;
}
/**@}*/

/** @name MQTT packet unpacking functions */
/**@{*/
/**
 * @brief Unpacks an MQTT CONNECT packet from a byte buffer
 *
 * This function parses the CONNECT packet and fills the provided packet
 * structure with the extracted data. It allocates memory for variable-length
 * fields which must be freed by the caller.
 *
 * @param[in] buf Pointer to the buffer containing the packet
 * @param[in] hdr Pointer to the parsed packet header
 * @param[out] pkt Pointer to store the unpacked packet
 * @return Size of the unpacked packet in bytes
 */
static size_t unpack_mqtt_connect(const unsigned char *buf,
                                 union mqtt_header *hdr,
                                 union mqtt_packet *pkt)
{
    struct mqtt_connect connect = { .header = *hdr };
    pkt->connect = connect;
    const unsigned char *init = buf;
    size_t len = mqtt_decode_length(&buf);
    buf = init + 8;
    pkt->connect.byte = unpack_u8((const uint8_t **)&buf);
    pkt->connect.payload.keepalive = unpack_u16((const uint8_t **)&buf);
    uint16_t cid_len = unpack_u16((const uint8_t **)&buf);
    if (cid_len > 0) {
        pkt->connect.payload.client_id = malloc(cid_len + 1);
        unpack_bytes((const uint8_t **)&buf, cid_len,
                    pkt->connect.payload.client_id);
    }
    if (pkt->connect.bits.will == 1) {
        unpack_string16((uint8_t **)&buf,
                       &pkt->connect.payload.will_topic);
        unpack_string16((uint8_t **)&buf,
                       &pkt->connect.payload.will_message);
    }
    if (pkt->connect.bits.username == 1)
        unpack_string16((uint8_t **)&buf,
                       &pkt->connect.payload.username);
    if (pkt->connect.bits.password == 1)
        unpack_string16((uint8_t **)&buf,
                       &pkt->connect.payload.password);
    return len;
}
