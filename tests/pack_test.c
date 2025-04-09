#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/pack.h"

void test_pack_unpack_u8(void)
{
	printf("Testing pack_u8 and unpack_u8...\n");

	// Initialize buffer for packing
	uint8_t buffer[10] = { 0 };
	uint8_t *buf_ptr = buffer;

	// Test values
	uint8_t values[] = { 0, 1, 127, 128, 255 };
	int num_values = sizeof(values) / sizeof(values[0]);

	// Pack values
	for (int i = 0; i < num_values; i++) {
		pack_u8(&buf_ptr, values[i]);
	}

	// Reset pointer for unpacking
	const uint8_t *unpack_ptr = buffer;

	// Unpack and verify
	for (int i = 0; i < num_values; i++) {
		uint8_t unpacked = unpack_u8(&unpack_ptr);
		printf("  Value %d: packed %u, unpacked %u\n", i, values[i],
		       unpacked);
		assert(unpacked == values[i]);
	}

	// Verify buffer pointer position
	assert(buf_ptr - buffer == num_values);
	assert(unpack_ptr - buffer == num_values);

	printf("✓ pack_u8 and unpack_u8 tests passed\n\n");
}

void test_pack_unpack_u16(void)
{
	printf("Testing pack_u16 and unpack_u16...\n");

	// Initialize buffer for packing
	uint8_t buffer[20] = { 0 };
	uint8_t *buf_ptr = buffer;

	// Test values (including edge cases)
	uint16_t values[] = { 0, 1, 255, 256, 32767, 32768, 65535 };
	int num_values = sizeof(values) / sizeof(values[0]);

	// Pack values
	for (int i = 0; i < num_values; i++) {
		pack_u16(&buf_ptr, values[i]);
	}

	// Reset pointer for unpacking
	const uint8_t *unpack_ptr = buffer;

	// Unpack and verify
	for (int i = 0; i < num_values; i++) {
		uint16_t unpacked = unpack_u16(&unpack_ptr);
		printf("  Value %d: packed %u, unpacked %u\n", i, values[i],
		       unpacked);
		assert(unpacked == values[i]);
	}

	// Verify buffer pointer position
	assert(buf_ptr - buffer == num_values * sizeof(uint16_t));
	assert(unpack_ptr - buffer == num_values * sizeof(uint16_t));

	printf("✓ pack_u16 and unpack_u16 tests passed\n\n");
}

void test_pack_unpack_u32(void)
{
	printf("Testing pack_u32 and unpack_u32...\n");

	// Initialize buffer for packing
	uint8_t buffer[40] = { 0 };
	uint8_t *buf_ptr = buffer;

	// Test values (including edge cases)
	uint32_t values[] = { 0,     1,		 255,	      65535,
			      65536, 2147483647, 2147483648U, 4294967295U };
	int num_values = sizeof(values) / sizeof(values[0]);

	// Pack values
	for (int i = 0; i < num_values; i++) {
		pack_u32(&buf_ptr, values[i]);
	}

	// Reset pointer for unpacking
	const uint8_t *unpack_ptr = buffer;

	// Unpack and verify
	for (int i = 0; i < num_values; i++) {
		uint32_t unpacked = unpack_u32(&unpack_ptr);
		printf("  Value %d: packed %u, unpacked %u\n", i, values[i],
		       unpacked);
		assert(unpacked == values[i]);
	}

	// Verify buffer pointer position
	assert(buf_ptr - buffer == num_values * sizeof(uint32_t));
	assert(unpack_ptr - buffer == num_values * sizeof(uint32_t));

	printf("✓ pack_u32 and unpack_u32 tests passed\n\n");
}

void test_pack_unpack_bytes(void)
{
	printf("Testing pack_bytes and unpack_bytes...\n");

	// Test strings
	const char *test_strings[] = {
		"", "a", "Hello, World!",
		"This is a longer test string with some numbers 123456789"
	};
	int num_strings = sizeof(test_strings) / sizeof(test_strings[0]);

	for (int i = 0; i < num_strings; i++) {
		// Initialize buffer with zeros
		size_t buffer_size =
			strlen(test_strings[i]) + 10; // Extra space
		uint8_t *buffer = calloc(buffer_size, 1);
		uint8_t *buf_ptr = buffer;

		// Pack string
		pack_bytes(&buf_ptr, (uint8_t *)test_strings[i]);

		// Prepare buffer for unpacked result
		uint8_t *result = calloc(strlen(test_strings[i]) + 1, 1);

		// Reset pointer for unpacking
		const uint8_t *unpack_ptr = buffer;

		// Unpack bytes
		unpack_bytes(&unpack_ptr, strlen(test_strings[i]), result);

		// Verify result
		printf("  String %d: packed \"%s\", unpacked \"%s\"\n", i,
		       test_strings[i], result);
		assert(strcmp((char *)result, test_strings[i]) == 0);

		// Verify pointer positions
		assert(buf_ptr - buffer == strlen(test_strings[i]));
		assert(unpack_ptr - buffer == strlen(test_strings[i]));

		// Clean up
		free(buffer);
		free(result);
	}

	printf("✓ pack_bytes and unpack_bytes tests passed\n\n");
}

void test_unpack_string16(void)
{
	printf("Testing unpack_string16...\n");

	// Test strings
	const char *test_strings[] = {
		"", "a", "Hello, World!",
		"This is a longer test string with some numbers 123456789"
	};
	int num_strings = sizeof(test_strings) / sizeof(test_strings[0]);

	for (int i = 0; i < num_strings; i++) {
		// Calculate buffer size: 2 bytes for length + string length
		size_t string_len = strlen(test_strings[i]);
		size_t buffer_size = sizeof(uint16_t) + string_len;
		uint8_t *buffer = calloc(buffer_size, 1);
		uint8_t *buf_ptr = buffer;

		// Pack string length and string
		pack_u16(&buf_ptr, string_len);
		memcpy(buf_ptr, test_strings[i], string_len);
		buf_ptr += string_len;

		// Reset pointer for unpacking
		uint8_t *unpack_ptr = buffer;
		uint8_t *result = NULL;

		// Unpack string
		uint16_t unpacked_len = unpack_string16(&unpack_ptr, &result);

		// Verify result
		printf("  String %d: length %zu, unpacked length %u\n", i,
		       string_len, unpacked_len);
		printf("  Original: \"%s\", Unpacked: \"%s\"\n",
		       test_strings[i], result);
		assert(unpacked_len == string_len);
		assert(strcmp((char *)result, test_strings[i]) == 0);

		// Verify buffer pointer position
		assert(unpack_ptr - buffer == buffer_size);

		// Clean up
		free(buffer);
		free(result);
	}

	printf("✓ unpack_string16 test passed\n\n");
}

void test_combined_operations(void)
{
	printf("Testing combined packing and unpacking operations...\n");

	// Initialize buffer
	uint8_t buffer[100] = { 0 };
	uint8_t *buf_ptr = buffer;

	// Pack various values
	uint8_t u8_val = 42;
	uint16_t u16_val = 12345;
	uint32_t u32_val = 987654321;
	uint8_t *str = (uint8_t *)"Test String";

	pack_u8(&buf_ptr, u8_val);
	pack_u16(&buf_ptr, u16_val);
	pack_u32(&buf_ptr, u32_val);
	pack_bytes(&buf_ptr, str);

	// Calculate used buffer size
	size_t used_size = buf_ptr - buffer;

	// Reset pointer for unpacking
	const uint8_t *unpack_ptr = buffer;

	// Unpack and verify
	uint8_t unpacked_u8 = unpack_u8(&unpack_ptr);
	uint16_t unpacked_u16 = unpack_u16(&unpack_ptr);
	uint32_t unpacked_u32 = unpack_u32(&unpack_ptr);

	uint8_t unpacked_str[20] = { 0 };
	unpack_bytes(&unpack_ptr, strlen((char *)str), unpacked_str);

	printf("  u8: packed %u, unpacked %u\n", u8_val, unpacked_u8);
	printf("  u16: packed %u, unpacked %u\n", u16_val, unpacked_u16);
	printf("  u32: packed %u, unpacked %u\n", u32_val, unpacked_u32);
	printf("  string: packed \"%s\", unpacked \"%s\"\n", str, unpacked_str);

	assert(unpacked_u8 == u8_val);
	assert(unpacked_u16 == u16_val);
	assert(unpacked_u32 == u32_val);
	assert(strcmp((char *)unpacked_str, (char *)str) == 0);

	// Verify final pointer position
	assert(unpack_ptr - buffer == used_size);

	printf("✓ Combined operations test passed\n\n");
}

void test_error_cases(void)
{
	printf("Testing error handling and edge cases...\n");

	// Test unpack_string16 with zero length
	{
		uint8_t buffer[2] = { 0 }; // length of 0
		uint8_t *buf_ptr = buffer;
		uint8_t *result = NULL;

		uint16_t len = unpack_string16(&buf_ptr, &result);

		printf("  Zero-length string: length %u\n", len);
		assert(len == 0);
		assert(result != NULL);
		assert(result[0] == '\0');

		free(result);
	}

	printf("✓ Error handling tests passed\n\n");
}

int main(void)
{
	printf("Running pack module unit tests\n");
	printf("=============================\n\n");

	test_pack_unpack_u8();
	test_pack_unpack_u16();
	test_pack_unpack_u32();
	test_pack_unpack_bytes();
	test_unpack_string16();
	test_combined_operations();
	test_error_cases();

	printf("All tests passed!\n");
	return 0;
}
