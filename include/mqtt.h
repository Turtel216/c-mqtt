/**
 * @file mqtt.h
 * @brief The MQTT module defines utilities for handling MQTT communications
 */

#ifndef MQTT_H_
#define MQTT_H_

#include <stddef.h>
#include <stdio.h>

/** @name MQTT Header Constants */
/**@{*/
/** Fixed size of MQTT fixed header */
#define HEADER_LEN 2
/** Fixed size of every MQTT ACK */
#define ACK_LEN 4
/**@}*/

/** @name MQTT Message Type Bytes */
/**@{*/
/** CONNACK message type byte */
#define CONNACK_BYTE 0x20
/** PUBLISH message type byte */
#define PUBLISH_BYTE 0x30
/** PUBACK message type byte */
#define PUBACK_BYTE 0x40
/** PUBREC message type byte */
#define PUBREC_BYTE 0x50
/** PUBREL message type byte */
#define PUBREL_BYTE 0x60
/** PUBCOMP message type byte */
#define PUBCOMP_BYTE 0x70
/** SUBACK message type byte */
#define SUBACK_BYTE 0x90
/** UNSUBACK message type byte */
#define UNSUBACK_BYTE 0xB0
/** PINGRESP message type byte */
#define PINGRESP_BYTE 0xD0
/**@}*/

/**
 * @brief MQTT message types as defined in the MQTT protocol
 */
enum packet_type {
	CONNECT = 1,    /**< Client request to connect to server */
	CONNACK,        /**< Server acknowledgment of connection */
	PUBLISH,        /**< Publish message */
	PUBACK,         /**< Publish acknowledgment */
	PUBREC,         /**< Publish received (QoS 2) */
	PUBREL,         /**< Publish release (QoS 2) */
	PUBCOMP,        /**< Publish complete (QoS 2) */
	SUBSCRIBE,      /**< Client subscribe request */
	SUBACK,         /**< Server subscribe acknowledgment */
	UNSUBSCRIBE,    /**< Client unsubscribe request */
	UNSUBACK,       /**< Server unsubscribe acknowledgment */
	PINGREQ,        /**< Client ping request */
	PINGRESP,       /**< Server ping response */
	DISCONNECT      /**< Client disconnection notification */
};

/**
 * @brief Quality of Service levels for MQTT messaging
 */
enum qos_level {
	AT_MOST_ONCE,   /**< QoS 0: Message delivered at most once (fire and forget) */
	AT_LEAST_ONCE,  /**< QoS 1: Message delivered at least once (acknowledged delivery) */
	EXACTLY_ONCE    /**< QoS 2: Message delivered exactly once (assured delivery) */
};

/**
 * @brief Byte representation of an MQTT header
 */
union mqtt_header {
	unsigned char byte;           /**< Raw byte value */
	struct {
		unsigned retain : 1;  /**< Retain flag */
		unsigned qos : 2;     /**< Quality of Service level */
		unsigned dup : 1;     /**< Duplicate delivery flag */
		unsigned type : 4;    /**< Message type */
	} bits;
};

/**
 * @brief MQTT CONNECT request structure
 */
struct mqtt_connect {
	union mqtt_header header;     /**< MQTT header */
	union {
		unsigned char byte;   /**< Raw byte value */
		struct {              /**< Byte describing payload */
			int reserved : 1;
			unsigned clean_session : 1;  /**< Clean session flag */
			unsigned will : 1;           /**< Will flag */
			unsigned will_qos : 2;       /**< Will QoS level */
			unsigned will_retain : 1;    /**< Will retain flag */
			unsigned password : 1;       /**< Password flag */
			unsigned username : 1;       /**< Username flag */
		} bits;
		struct {              /**< Struct representation of payload */
			unsigned short keepalive;     /**< Keep-alive timer value in seconds */
			unsigned char *client_id;     /**< Client identifier */
			unsigned char *username;      /**< Username if username flag set */
			unsigned char *password;      /**< Password if password flag set */
			unsigned char *will_topic;    /**< Will topic if will flag set */
			unsigned char *will_message;  /**< Will message if will flag set */
		} payload;
	};
};

/**
 * @brief MQTT CONNACK response structure
 */
struct mqtt_connack {
	union mqtt_header header;     /**< MQTT Header */
	union {
		unsigned char byte;   /**< Raw byte value */
		struct {
			unsigned session_present : 1;  /**< Session present flag */
			unsigned reserved : 2;         /**< Reserved bits */
		} bits;
		unsigned char rc;     /**< Return Code */
	};
};

/**
 * @brief MQTT SUBSCRIBE request structure
 */
struct mqtt_subscribe {
	union mqtt_header header;     /**< MQTT header */
	unsigned short pkt_id;        /**< Package id */
	unsigned short tuples_len;    /**< Number of topic tuples */
	struct {
		unsigned short topic_len;  /**< Length of topic string */
		unsigned char *topic;      /**< Topic String */
		unsigned qos;              /**< Requested QoS level */
	} *tuples;                    /**< Array of topic tuples */
};

/**
 * @brief MQTT UNSUBSCRIBE request structure
 */
struct mqtt_unsubscribe {
	union mqtt_header header;     /**< MQTT header */
	unsigned short pkt_id;        /**< Package id */
	unsigned short tuples_len;    /**< Number of topic tuples */
	struct {
		unsigned short topic_len;  /**< Length of topic string */
		unsigned char *topic;      /**< Topic String */
	} *tuples;                    /**< Array of topic tuples */
};

/**
 * @brief Packet definition for SUBSCRIBE and PUBLISH acknowledgments
 */
struct mqtt_suback {
	union mqtt_header header;     /**< MQTT header */
	unsigned short pkt_id;        /**< Package id */
	unsigned short rcslen;        /**< Return Code String length */
	unsigned char *rcs;           /**< Return Code String */
};

/**
 * @brief MQTT PUBLISH request structure
 */
struct mqtt_publish {
	union mqtt_header header;     /**< MQTT header */
	unsigned short pkt_id;        /**< Package id */
	unsigned short topiclen;      /**< Topic String length */
	unsigned char *topic;         /**< Topic String */
	unsigned short payloadlen;    /**< Payload String length */
	unsigned char *payload;       /**< Payload String */
};

/**
 * @brief Generic MQTT acknowledgment packet structure
 */
struct mqtt_ack {
	union mqtt_header header;     /**< MQTT header */
	unsigned short pkt_id;        /**< Package id */
};

/** @name MQTT specialized acknowledgment packet types */
/**@{*/
/** PUBACK packet type */
typedef struct mqtt_ack mqtt_puback;
/** PUBREC packet type */
typedef struct mqtt_ack mqtt_pubrec;
/** PUBREL packet type */
typedef struct mqtt_ack mqtt_pubrel;
/** PUBCOMP packet type */
typedef struct mqtt_ack mqtt_pubcomp;
/** UNSUBACK packet type */
typedef struct mqtt_ack mqtt_unsuback;
/** PINGREQ packet type */
typedef union mqtt_header mqtt_pingreq;
/** PINGRESP packet type */
typedef union mqtt_header mqtt_pingresp;
/** DISCONNECT packet type */
typedef union mqtt_header mqtt_disconnect;
/**@}*/

/**
 * @brief MQTT packet union for representing any MQTT packet type
 */
union mqtt_packet {
	struct mqtt_ack ack;            /**< Generic acknowledgment structure */
	union mqtt_header header;       /**< MQTT header */
	struct mqtt_connect connect;    /**< CONNECT packet */
	struct mqtt_connack connack;    /**< CONNACK packet */
	struct mqtt_suback suback;      /**< SUBACK packet */
	struct mqtt_publish publish;    /**< PUBLISH packet */
	struct mqtt_subscribe subscribe;    /**< SUBSCRIBE packet */
	struct mqtt_unsubscribe unsubscribe;  /**< UNSUBSCRIBE packet */
};

/**
 * @brief Encodes the Remaining Length field of an MQTT packet header
 *
 * @param[out] buf Buffer to store the encoded length
 * @param[in] len Length value to encode
 * @return Number of bytes used for encoding the length
 */
int mqtt_encode_length(unsigned char *, size_t);

/**
 * @brief Decodes the Remaining Length field of an MQTT packet header
 *
 * @param[in,out] buf Pointer to the buffer containing the encoded length
 * @return Decoded length value
 */
unsigned long long mqtt_decode_length(const unsigned char **);

/**
 * @brief Unmarshals an MQTT packet
 *
 * @param[in] packet Pointer to the MQTT packet to unmarshal
 * @param[in] type Type of the MQTT packet
 * @return Status code indicating success or failure
 */
int unpack_mqtt_packet(const union mqtt_packet *, unsigned);

/**
 * @brief Marshals an MQTT packet
 *
 * @param[in] packet Pointer to the MQTT packet to marshal
 * @param[in] type Type of the MQTT packet
 * @return Pointer to the marshaled packet buffer (must be freed by caller)
 */
unsigned char *packet_mqtt_packet(const union mqtt_packaet *, unsigned);

/**
 * @brief Utility function that creates an MQTT packet header
 *
 * @param[in] byte Header byte value
 * @return Pointer to the created MQTT header
 */
union mqtt_header *mqtt_packet_header(unsigned char);

/**
 * @brief Utility function that creates an MQTT ACK packet
 *
 * @param[in] byte Header byte value
 * @param[in] pkt_id Packet identifier
 * @return Pointer to the created MQTT ACK packet
 */
struct mqtt_ack *mqtt_packet_ack(unsigned char, unsigned short);

/**
 * @brief Utility function that creates an MQTT CONNACK response
 *
 * @param[in] byte Header byte value
 * @param[in] flags Connection flags
 * @param[in] rc Return code
 * @return Pointer to the created MQTT CONNACK packet
 */
struct mqtt_connack *mqtt_packet_connack(unsigned char, unsigned char,
					 unsigned char);

/**
 * @brief Utility function that creates an MQTT SUBACK packet
 *
 * @param[in] byte Header byte value
 * @param[in] pkt_id Packet identifier
 * @param[in] rcs Return codes
 * @param[in] rcslen Length of return codes
 * @return Pointer to the created MQTT SUBACK packet
 */
struct mqtt_suback *mqtt_packet_suback(unsigned char, unsigned short,
				       unsigned char *, unsigned short);

/**
 * @brief Utility function that creates an MQTT PUBLISH packet
 *
 * @param[in] byte Header byte value
 * @param[in] pkt_id Packet identifier
 * @param[in] topiclen Length of topic
 * @param[in] topic Topic string
 * @param[in] payloadlen Length of payload
 * @param[in] payload Payload data
 * @return Pointer to the created MQTT PUBLISH packet
 */
struct mqtt_publish *mqtt_packet_publish(unsigned char, unsigned short, size_t,
					 unsigned char *, size_t,
					 unsigned char *);

/**
 * @brief Utility function that releases an MQTT packet and frees up its memory
 *
 * @param[in] packet Pointer to the MQTT packet to release
 * @param[in] type Type of the MQTT packet
 */
void mqtt_packet_release(union mqtt_packet *, unsigned);

#endif // MQTT_H_
