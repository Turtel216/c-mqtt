// The MQTT module defines utilities for handling MQTT communications

#ifndef MQTT_H_
#define MQTT_H_

#include <cstddef>
#include <stdio.h>

// Fixed size ot MQTT fixed header
#define HEADER_LEN 2
// Fixed size of every MQTT ACK
#define ACK_LEN 4

// Stub bytes
#define CONNACK_BYTE 0x20
#define PUBLISH_BYTE 0x30
#define PUBACK_BYTE 0x40
#define PUBREC_BYTE 0x50
#define PUBREL_BYTE 0x60
#define PUBCOMP_BYTE 0x70
#define SUBACK_BYTE 0x90
#define UNSUBACK_BYTE 0xB0
#define PINGRESP_BYTE 0xD0

// Message types
enum packet_type {
	CONNECT = 1,
	CONNACK,
	PUBLISH,
	PUBACK,
	PUBREC,
	PUBREL,
	PUBCOMP,
	SUBSCRIBE,
	SUBACK,
	UNSUBSCRIBE,
	UNSUBACK,
	PINGREQ,
	PINGRESP,
	DISCONNECT
};

enum qos_level { AT_MOST_ONCE, AT_LEAST_ONCE, EXACTLY_ONCE };

// Byte representation of an MQTT header
union mqtt_header {
	unsigned char byte;
	struct {
		unsigned retain : 1;
		unsigned qos : 2;
		unsigned dup : 1;
		unsigned type : 4;
	} bits;
};

// MQTT CONNECT request
struct mqtt_connect {
	union mqtt_header header; // MQTT header
	union {
		unsigned char byte;
		struct { // Byte describing payload
			int reserved : 1;
			unsigned clean_session : 1;
			unsigned will : 1;
			unsigned will_qos : 2;
			unsigned will_retain : 1;
			unsigned password : 1;
			unsigned username : 1;
		} bits;
		struct { // Struct representaion of payload
			unsigned short keepalive;
			unsigned char *client_id;
			unsigned char *username;
			unsigned char *password;
			unsigned char *will_topic;
			unsigned char *will_message;
		} payload;
	};
};

// MQTT CONNACK response
struct mqtt_connack {
	union mqtt_header header; // MQTT Header
	union {
		unsigned char byte;
		struct {
			unsigned session_present : 1;
			unsigned reserved : 2;
		} bits;
		unsigned char rc; // Return Code
	};
};

// MQTT SUBSCRIBE request
struct mqtt_subscribe {
	union mqtt_header header;
	unsigned short pkt_id; // Package id
	unsigned short tuples_len;
	struct {
		unsigned short topic_len; // Length of topic string
		unsigned char *topic; // Topic String
		unsigned qos;
	} *tuples;
};

// MQTT UNSUBSCRIBE request
struct mqtt_unsubscribe {
	union mqtt_header header;
	unsigned short pkt_id; // Package id
	unsigned short tuples_len;
	struct {
		unsigned short topic_len; // Length of topic string
		unsigned char *topic; // Topic String
	} *tuples;
};

// Packet definition for SUBSCRIBE and PUBLISH
struct mqtt_suback {
	union mqtt_header header;
	unsigned short pkt_id; // Package id
	unsigned short rcslen; // Return Code String length
	unsigned char *rcs; // Return Code String
};

// MQTT PUBLISH request
struct mqtt_publish {
	union mqtt_header header;
	unsigned short pkt_id; // Package id
	unsigned short topiclen; // Topic String length
	unsigned char *topic; // Topic String
	unsigned short payloadlen; // Payload String length
	unsigned char *payload; // Payload String
};

// Generic MQTT packet
struct mqtt_ack {
	union mqtt_header header;
	unsigned short pkt_id; // Package id
};

// ACK packets for:
// PUBACK
typedef struct mqtt_ack mqtt_puback;
// PUBREC
typedef struct mqtt_ack mqtt_pubrec;
// PUBREL
typedef struct mqtt_ack mqtt_pubrel;
// PUBCOMP
typedef struct mqtt_ack mqtt_pubcomp;
// UNSUBACK
typedef struct mqtt_ack mqtt_unsuback;
// PINGREQ
typedef union mqtt_header mqtt_pingreq;
// PINGRESP
typedef union mqtt_header mqtt_pingresp;
// DISCONNECT
typedef union mqtt_header mqtt_disconnect;

// MQTT packet
union mqtt_packet {
	struct mqtt_ack ack;
	union mqtt_header header;
	struct mqtt_connect connect;
	struct mqtt_connack connack;
	struct mqtt_suback suback;
	struct mqtt_publish publish;
	struct mqtt_subscribe subscribe;
	struct mqtt_unsubscribe unsubscribe;
};

// Returns the length of an encoded mqtt byte stream
int mqtt_encode_length(unsigned char *, size_t);
// Returns the length of an decoded mqtt byte stream
unsigned long long mqtt_decode_length(const unsigned char **);
// Marshals an mqtt packet
int unpack_mqtt_packet(const union mqtt_packet *, unsigned);
// Unmarshals an mqtt packet
unsigned char *packet_mqtt_packet(const union mqtt_packaet *, unsigned);

// Utility function that returns the mqtt packet header
union mqtt_header *mqtt_packet_header(unsigned char);
// Utility function that returns the mqtt packet ACT
struct mqtt_ack *mqtt_packet_ack(unsigned char, unsigned short);
// Utility function that returns an mqtt CONNACK response
struct mqtt_connack *mqtt_packet_connack(unsigned char, unsigned char,
					 unsigned char);
// Utility function that returns an mqtt SUBACK request
struct mqtt_suback *mqtt_packet_suback(unsigned char, unsigned short,
				       unsigned char *, unsigned short);
// Utility function that returns an mqtt  request
struct mqtt_publish *mqtt_packet_publish(unsigned char, unsigned short, size_t,
					 unsigned char *, size_t,
					 unsigned char *);
// Utility function that releases an mqtt packet and frees up its memory
void mqtt_packet_release(union mqtt_packet *, unsigned);

#endif // MQTT_H_
