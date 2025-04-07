#ifndef MQTT_H_
#define MQTT_H_

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

// Struct representing an MQTT CONNECT request
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

// Struct representing an MQTT CONNAKT response
struct mqtt_connakt {
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

// Struct reprenting an MQTT SUBSCRIBE request
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

// Struct repesentin an MQTT UNSUBSCRIBE request
struct mqtt_unsubscribe {
	union mqtt_header header;
	unsigned short pkt_id; // Package id
	unsigned short tuples_len;
	struct {
		unsigned short topic_len; // Length of topic string
		unsigned char *topic; // Topic String
	} *tuples;
};
#endif // MQTT_H_
