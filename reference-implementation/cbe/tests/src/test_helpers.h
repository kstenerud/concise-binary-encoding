#pragma once

#include <gtest/gtest.h>
#include "cbe_encoder.h"
#include "cbe_decoder.h"
#include "test_utils.h"

namespace cbe_test
{

bool expect_encode_produces_data_and_status(
	int buffer_size,
	std::shared_ptr<enc::encoding> encoding,
	const std::vector<uint8_t> expected_memory,
	cbe_encode_status expected_status);

void expect_decode_produces_data_and_status(
	int buffer_size,
	const std::vector<uint8_t> memory,
	std::shared_ptr<enc::encoding> expected_encoding,
	cbe_decode_status expected_status);

void expect_encode_decode_produces_data_and_status(
    int buffer_size,
    std::shared_ptr<enc::encoding> expected_encoding,
    const std::vector<uint8_t> expected_memory,
    cbe_encode_status expected_encode_status,
    cbe_decode_status expected_decode_status);

void expect_encode_decode_with_shrinking_buffer_size(
	int min_buffer_size,
	std::shared_ptr<enc::encoding> expected_encoding,
	const std::vector<uint8_t> expected_memory);

void expect_encode_decode_produces_status(
	std::shared_ptr<enc::encoding> encoding,
	cbe_encode_status expected_encode_status,
	cbe_decode_status expected_decode_status);

void expect_encode_produces_status(
    std::shared_ptr<enc::encoding> encoding,
    cbe_encode_status expected_encode_status);

void expect_decode_produces_status(
    std::vector<uint8_t> document,
    cbe_decode_status expected_decode_status);

void expect_decode_stop_in_callback(
    std::shared_ptr<enc::encoding> encoding);

} // namespace cbe_test


// Test that the specified encoding produces the expected memory.
#define TEST_ENCODE(TESTCASE, NAME, BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
	std::vector<uint8_t> memory = __VA_ARGS__; \
	cbe_test::expect_encode_produces_data_and_status(BUFFER_SIZE, ENCODING, memory, CBE_ENCODE_STATUS_OK); \
}

// Test that the specified memory decodes into the equivalent of the expected encoding.
#define TEST_DECODE(TESTCASE, NAME, BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
	std::vector<uint8_t> memory = __VA_ARGS__; \
	cbe_test::expect_decode_produces_data_and_status(BUFFER_SIZE, memory, ENCODING, CBE_DECODE_STATUS_OK); \
}

// Test that encoding and decoding results in success.
#define TEST_ENCODE_DECODE(TESTCASE, NAME, BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
	std::vector<uint8_t> memory = __VA_ARGS__; \
	cbe_test::expect_encode_decode_produces_data_and_status(BUFFER_SIZE, ENCODING, memory, CBE_ENCODE_STATUS_OK, CBE_DECODE_STATUS_OK); \
}

// Test that encoding and decoding produces the specified memory, and reproduces the equivalent encoding.
#define TEST_ENCODE_DECODE_SHRINKING(TESTCASE, NAME, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
	cbe_test::expect_encode_decode_with_shrinking_buffer_size(0, ENCODING, __VA_ARGS__); \
}

// Test that encoding and decoding produces the specified memory, and reproduces the equivalent encoding.
#define TEST_ENCODE_DECODE_SHRINKING_CONTAINER(TESTCASE, NAME, MIN_BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
	cbe_test::expect_encode_decode_with_shrinking_buffer_size(MIN_BUFFER_SIZE, ENCODING, __VA_ARGS__); \
}

// Test that encoding and decoding results in the specified status codes.
#define TEST_ENCODE_DECODE_STATUS(TESTCASE, NAME, EXPECTED_ENCODE_STATUS, EXPECTED_DECODE_STATUS, ENCODING) \
TEST(TESTCASE, NAME) \
{ \
	cbe_test::expect_encode_decode_produces_status(ENCODING, EXPECTED_ENCODE_STATUS, EXPECTED_DECODE_STATUS); \
}

// Test that encoding results in the specified status code.
#define TEST_ENCODE_STATUS(TESTCASE, NAME, EXPECTED_ENCODE_STATUS, ENCODING) \
TEST(TESTCASE, NAME) \
{ \
	cbe_test::expect_encode_produces_status(ENCODING, EXPECTED_ENCODE_STATUS); \
}

// Test that decoding results in the specified status code.
#define TEST_DECODE_STATUS(TESTCASE, NAME, EXPECTED_DECODE_STATUS, ...) \
TEST(TESTCASE, NAME) \
{ \
	cbe_test::expect_decode_produces_status(__VA_ARGS__, EXPECTED_DECODE_STATUS); \
}

// Test that decoding results in the specified status code.
#define TEST_STOP_IN_CALLBACK(TESTCASE, NAME, ...) \
TEST(TESTCASE, NAME) \
{ \
	cbe_test::expect_decode_stop_in_callback(__VA_ARGS__); \
}