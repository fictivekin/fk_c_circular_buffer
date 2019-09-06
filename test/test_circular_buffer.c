#include "fk_circular_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

void test_init() {
	circularBuffer_t buf;
	int ret;
	char foo[32];
	ret = circularBuffer_init(&buf, foo, sizeof(foo), 15);
	assert(ret == CIRC_BUF_SIZE_ERROR);

	ret = circularBuffer_init(&buf, 0, 0, 0);
	assert(ret == CIRC_BUF_SIZE_ERROR);

	ret = circularBuffer_init(&buf, foo, 0, 0);
	assert(ret == CIRC_BUF_SIZE_ERROR);

	ret = circularBuffer_init(&buf, foo, 16, 0);
	assert(ret == CIRC_BUF_SIZE_ERROR);

	ret = circularBuffer_init(&buf, foo, sizeof(foo), 16);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_is_empty(&buf);
	assert(ret);

	ret = circularBuffer_is_full(&buf);
	assert(!ret);
}

void test_push_push() {
	circularBuffer_t buf;
	int ret;
	char foo[8];

	ret = circularBuffer_init(&buf, foo, sizeof(foo), 16);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_is_empty(&buf);
	assert(ret);

	ret = circularBuffer_is_full(&buf);
	assert(!ret);
}

void test_push_peek_pop() {
	circularBuffer_t buf;
	int ret;
	size_t res;
	char buf_storage[32];
	char output[32];
	const size_t recordCount = 4;
	const size_t recordSize = sizeof(output) / recordCount;
	unsigned int i;
	for (i = 0; i < sizeof(output); i++) {
		output[i] = i;
	}
	ret = circularBuffer_init(&buf, buf_storage, sizeof(buf_storage), recordSize);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_popFIFO(&buf, output, memcpy);
	assert(ret == CIRC_BUF_BUFFER_EMPTY);

	ret = circularBuffer_peek(&buf, output, 1, memcpy);
	assert(ret == CIRC_BUF_BUFFER_EMPTY);

	for (i = 0; i < recordCount; i++) {
		ret = circularBuffer_push(&buf, output, NULL);
		assert(ret == CIRC_BUF_NO_ERROR);
	}
	circularBuffer_getCount(&buf, &res);
	assert(res == recordCount);

	ret = circularBuffer_push(&buf, output, NULL);
	assert(ret == CIRC_BUF_BUFFER_FULL);

	ret = circularBuffer_peek(&buf, output, 1, NULL);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_peek(&buf, output, 4, NULL);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_peek(&buf, output, 5, NULL);
	assert(ret == CIRC_BUF_SIZE_ERROR);

	ret = circularBuffer_popFIFO(&buf, output, NULL);
	assert(ret == CIRC_BUF_NO_ERROR);

	circularBuffer_getCount(&buf, &res);
	assert(res == recordCount - 1);

	ret = circularBuffer_peek(&buf, output, recordCount, memcpy);
	assert(ret == CIRC_BUF_SIZE_ERROR);

	ret = circularBuffer_peek(&buf, output, recordCount - 1, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_peek(&buf, output, recordCount, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);
}

void test_wrapping_peek() {
	circularBuffer_t buf;
	int ret;
	char buf_storage[32];
	char output[32];
	char desired_output[32] = {
		8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		26, 27, 28, 29, 30, 31, 0, 1, 2, 3 ,4, 5, 6, 7,
	};
	const size_t recordCount = 4;
	const size_t recordSize = sizeof(output) / recordCount;
	unsigned int i;
	for (i = 0; i < sizeof(output); i++) {
		output[i] = i;
	}
	ret = circularBuffer_init(&buf, buf_storage, sizeof(buf_storage), recordSize);
	assert(ret == CIRC_BUF_NO_ERROR);

	for (i = 0; i < recordCount; i++) {
		ret = circularBuffer_push(&buf, output + i*recordSize, memcpy);
		assert(ret == CIRC_BUF_NO_ERROR);
	}

	memset(output, 0, sizeof(output));
	ret = circularBuffer_popFIFO(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_peek(&buf, output, recordCount, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);
	assert(memcmp(desired_output, output, sizeof(output)) == 0);
}

void test_wrapped_push() {
	circularBuffer_t buf;
	int ret;
	size_t res;
	char buf_storage[8];
	char output[8];

	const size_t recordCount = 4;
	const size_t recordSize = sizeof(output) / recordCount;
	unsigned int i;
	for (i = 0; i < sizeof(output); i++) {
		output[i] = i;
	}
	ret = circularBuffer_init(&buf, buf_storage, sizeof(buf_storage), recordSize);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push_n(&buf, output, 100, memcpy);
	assert(ret == CIRC_BUF_SIZE_ERROR);

	for (i = 0; i < 2; i++) {
		ret = circularBuffer_push(&buf, output + i*recordSize, memcpy);
		assert(ret == CIRC_BUF_NO_ERROR);
	}

	ret = circularBuffer_push_n(&buf, output, 3, memcpy);
	assert(ret == CIRC_BUF_BUFFER_FULL);
	circularBuffer_getCount(&buf, &res);
	assert(res == 2);

	ret = circularBuffer_popFIFO(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_popFIFO(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push_n(&buf, output, 3, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	circularBuffer_getCount(&buf, &res);
	assert(res == 3);
}

void test_pop_fifo_n() {
	circularBuffer_t buf;
	int ret;
	char buf_storage[8];
	char output[8];

	const size_t recordCount = 4;
	const size_t recordSize = sizeof(output) / recordCount;
	unsigned int i;
	for (i = 0; i < sizeof(output); i++) {
		output[i] = i;
	}
	ret = circularBuffer_init(&buf, buf_storage, sizeof(buf_storage), recordSize);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_popFIFO_n(&buf, output, 1, memcpy);
	assert(ret == CIRC_BUF_BUFFER_EMPTY);

	for (i = 0; i < 4; i++) {
		ret = circularBuffer_push(&buf, output + i*recordSize, memcpy);
		assert(ret == CIRC_BUF_NO_ERROR);
	}

	ret = circularBuffer_popFIFO_n(&buf, output, 2, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_is_full(&buf);
	assert(ret == true);

	ret = circularBuffer_popFIFO_n(&buf, output, 1, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_popFIFO_n(&buf, output, 3, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_popFIFO_n(&buf, output, 2, memcpy);
	assert(ret == CIRC_BUF_BUFFER_EMPTY);
}

void test_pop_lifo() {
	circularBuffer_t buf;
	int ret;
	char buf_storage[8];
	char output[8];
	char desired_output_1[] = {
		6, 7,
	};
	const size_t recordCount = 4;
	const size_t recordSize = sizeof(output) / recordCount;
	unsigned int i;
	for (i = 0; i < sizeof(output); i++) {
		output[i] = i;
	}
	ret = circularBuffer_init(&buf, buf_storage, sizeof(buf_storage), recordSize);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_popLIFO(&buf, output, memcpy);
	assert(ret == CIRC_BUF_BUFFER_EMPTY);

	ret = circularBuffer_push_n(&buf, output, 1, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);
	ret = circularBuffer_popLIFO(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push_n(&buf, output, 4, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);
	memset(output, 0, sizeof(output));

	ret = circularBuffer_popLIFO(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);
	assert(memcmp(output, desired_output_1, 2) == 0);
}

void test_pop_lifo_n() {
	circularBuffer_t buf;
	int ret;
	char buf_storage[8];
	char output[8];
	char desired_output_1[] = {
		4, 5, 6, 7,
	};
	char desired_output_2[] = {
		0, 1, 2, 3, 0, 1,
	};
	const size_t recordCount = 4;
	const size_t recordSize = sizeof(output) / recordCount;
	unsigned int i;
	for (i = 0; i < sizeof(output); i++) {
		output[i] = i;
	}
	ret = circularBuffer_init(&buf, buf_storage, sizeof(buf_storage), recordSize);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push_n(&buf, output, 4, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);
	memset(output, 0, sizeof(output));

	ret = circularBuffer_popLIFO_n(&buf, output, 2, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);
	assert(memcmp(output, desired_output_1, 4) == 0);

	output[0] = 0;
	output[1] = 1;
	ret = circularBuffer_push(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	memset(output, 0, sizeof(output));
	ret = circularBuffer_popLIFO_n(&buf, output, 3, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	assert(memcmp(output, desired_output_2, 6) == 0);

	ret = circularBuffer_popLIFO_n(&buf, output, 1, memcpy);
	assert(ret == CIRC_BUF_BUFFER_EMPTY);
}

void test_remove() {
	circularBuffer_t buf;
	int ret;
	char buf_storage[8];
	char output[8];

	const size_t recordCount = 4;
	const size_t recordSize = sizeof(output) / recordCount;
	unsigned int i;
	for (i = 0; i < sizeof(output); i++) {
		output[i] = i;
	}
	ret = circularBuffer_init(&buf, buf_storage, sizeof(buf_storage), recordSize);
	assert(ret == CIRC_BUF_NO_ERROR);
	for (i = 0; i < 4; i++) {
		ret = circularBuffer_push(&buf, output + i*recordSize, memcpy);
		assert(ret == CIRC_BUF_NO_ERROR);
	}
	ret = circularBuffer_remove_records(&buf, 1);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_remove_records(&buf, 2);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_push(&buf, output, memcpy);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_remove_records(&buf, 200);
	assert(ret == CIRC_BUF_NO_ERROR);

	ret = circularBuffer_remove_records(&buf, 2);
	assert(ret == CIRC_BUF_BUFFER_EMPTY);
}

void test_copy_buffer() {
	circularBuffer_t dst, src;
	char dst_storage[16];
	char src_storage[8];
	char data;
	int ret;

	circularBuffer_init(&dst, dst_storage, sizeof(dst_storage), 1);
	circularBuffer_init(&src, src_storage, sizeof(src_storage), 1);

	data = 1;
	ret = circularBuffer_push(&src, &data, NULL);
	data = 2;
	ret = circularBuffer_push(&src, &data, NULL);
	data = 3;
	ret = circularBuffer_push(&src, &data, NULL);
	data = 4;
	ret = circularBuffer_push(&src, &data, NULL);

	ret = circularBuffer_copy(&dst, &src, NULL);
	assert(ret == CIRC_BUF_NO_ERROR);
	assert(memcmp(src.p_data_location, dst.p_data_location, 4) == 0);


	circularBuffer_init(&dst, dst_storage, 2, 1);
	ret = circularBuffer_copy(&dst, &src, NULL);
	assert(ret == CIRC_BUF_SIZE_ERROR);

	ret = circularBuffer_copy(NULL, &src, NULL);
	assert(ret == CIRC_BUF_ADDR_ERROR);
}

void test_flush() {
	circularBuffer_t buf;
	int ret;
	char foo[32];
	size_t res;
	char data = 42;
	ret = circularBuffer_init(&buf, foo, sizeof(foo), 1);
	assert(ret == CIRC_BUF_NO_ERROR);
	ret = circularBuffer_push(&buf, &data, NULL);
	assert(ret == CIRC_BUF_NO_ERROR);
	ret = circularBuffer_getCount(&buf, &res);
	assert(ret == CIRC_BUF_NO_ERROR);
	assert(res == 1);
	ret = circularBuffer_flush(&buf);
	assert(ret == CIRC_BUF_NO_ERROR);
	ret = circularBuffer_getCount(&buf, &res);
	assert(ret == CIRC_BUF_NO_ERROR);
	assert(res == 0);
	ret = circularBuffer_max_slots(&buf, &res);
	assert(ret == CIRC_BUF_NO_ERROR);
	assert(res == sizeof(foo));
}

int main() {
	test_init();
	test_push_peek_pop();
	test_wrapping_peek();
	test_wrapped_push();
	test_pop_fifo_n();
	test_remove();
	test_pop_lifo_n();
	test_pop_lifo();
	test_copy_buffer();
	test_flush();
	return 0;
}
