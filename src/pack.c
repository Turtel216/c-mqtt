#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "../include/pack.h"

uint8_t unpack_u8(const uint8_t **);
uint16_t unpack_u16(const uint8_t **);
uint32_t unpack_u32(const uint8_t **);
uint8_t *unpack_bytes(const uint8_t **, size_t, uint8_t *);
uint16_t unpack_string16(uint8_t **buf, uint8_t **dest);
void pack_u8(uint8_t **, uint8_t);
void pack_u16(uint8_t **, uint16_t);
void pack_u32(uint8_t **, uint32_t);
void pack_bytes(uint8_t **, uint8_t *);

// Converts bytes to uint8
uint8_t unpack_u8(const uint8_t **buf)
{
	uint8_t val = **buf;
	(*buf)++;
	return val;
}

// Converts bytes to uint16
uint16_t unpack_u16(const uint8_t **buf)
{
	uint16_t val;
	memcpy(&val, *buf, sizeof(uint16_t));
	(*buf) += sizeof(uint16_t);
	return ntohs(val);
}

// Converts bytes to uint32
uint32_t unpack_u32(const uint8_t **buf)
{
	uint32_t val;
	memcpy(&val, *buf, sizeof(uint32_t));
	(*buf) += sizeof(uint32_t);
	return ntohl(val);
}

// Read a given length of bytes
uint8_t *unpack_bytes(const uint8_t **buf, size_t len, uint8_t *str)
{
	memcpy(str, *buf, len);
	str[len] = '\0';
	(*buf) += len;
	return str;
}

// Unpack a string prefixed by its length as a uint18 value
uint16_t unpack_string16(uint8_t **buf, uint8_t **dest)
{
	uint16_t len = unpack_u16(buf);
	*dest = malloc(len + 1);
	*dest = unpack_bytes(buf, len, *dest);
	return len;
}

// append a uint8_t -> bytes into the bytestring
void pack_u8(uint8_t **buf, uint8_t val)
{
	**buf = val;
	(*buf) += sizeof(uint8_t);
}

// append a uint16_t -> bytes into the bytestring
void pack_u16(uint8_t **buf, uint16_t val)
{
	uint16_t htonsval = htons(val);
	memcpy(*buf, &htonsval, sizeof(uint16_t));
	(*buf) += sizeof(uint16_t);
}

// append a uint32_t -> bytes into the bytestring
void pack_u32(uint8_t **buf, uint32_t val)
{
	uint32_t htonlval = htonl(val);
	memcpy(*buf, &htonlval, sizeof(uint32_t));
	(*buf) += sizeof(uint32_t);
}

// append len bytes into the bytestring
void pack_bytes(uint8_t **buf, uint8_t *str)
{
	size_t len = strlen((char *)str);
	memcpy(*buf, str, len);
	(*buf) += len;
}
