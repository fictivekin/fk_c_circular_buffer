#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fk_circular_buffer.h"

void free_buffer(circularBuffer_t *p_buffer) {
	free(p_buffer->p_data_location);
	p_buffer->p_data_location = NULL;
}

int init_buffer(circularBuffer_t *p_buffer, size_t buffer_size, size_t data_size) {
	void *storage;

	free_buffer(p_buffer);
	storage = malloc(buffer_size);

	return circularBuffer_init(p_buffer, storage, buffer_size, data_size);
}

int main(void) {
	char *line = NULL;
	size_t size;
	int ret;
	size_t data_size = 0;
	size_t buffer_size = 0;
	size_t peek_size;
	size_t pushNSize;
	size_t pop_fifo_size;
	size_t remove_size;
	size_t pop_lifo_size;
	circularBuffer_t buf;
	buf.p_data_location = NULL;
	void *pushData = NULL;
	void *peekData = NULL;
	void *pushNData = NULL;

	while(getline(&line, &size, stdin) != -1) {
		if (line[0] == 'i') { //init
			if (buf.p_data_location != NULL) continue;
			ret = sscanf(line, "i %zu %zu", &buffer_size, &data_size);
			if (ret == 2) {
				if (data_size > 4096 || buffer_size > 4096) continue;
				if (data_size == 0 || buffer_size == 0) continue;
				ret = init_buffer(&buf, buffer_size, data_size);
				free(pushData);
				pushData = malloc(data_size);
				if (ret != CIRC_BUF_NO_ERROR) {
					break;
				}
			}
		}
		if (!buf.p_data_location) continue;
		if (line[0] == 'L') { //flush
			circularBuffer_flush(&buf);
		} else if (line[0] == 'u') { //push
			ret = circularBuffer_push(&buf, pushData, memcpy);
		} else if (line[0] == 'E') { // pEek
			ret = sscanf(line, "E %zu", &peek_size);
			if (ret == 1) {
				if (peek_size > 1 << 16) continue;
				free(peekData);
				peekData = malloc(peek_size * data_size);
				ret = circularBuffer_peek(&buf, peekData, peek_size, memcpy);
			}
		} else if (line[0] == 'N') { // push N
			ret = sscanf(line, "N %zu", &pushNSize);
			if (ret == 1) {
				if (pushNSize > 1 << 16) continue;
				free(pushNData);
				pushNData = malloc(data_size * pushNSize);
				circularBuffer_push_n(&buf, pushNData, pushNSize, memcpy);
			}
		} else if (line[0] == 'o') { //pop fifo
			free(peekData);
			peekData = malloc(data_size);
			ret = circularBuffer_popFIFO(&buf, peekData, memcpy);
		} else if (line[0] == 'f') { // pop fifo N
			ret = sscanf(line, "f %zu", &pop_fifo_size);
			if (ret == 1) {
				if (pop_fifo_size > 1 << 16) continue;
				free(peekData);
				peekData = malloc(data_size * pop_fifo_size);
				ret = circularBuffer_popFIFO_n(&buf, peekData, pop_fifo_size, memcpy);
			}
		} else if (line[0] == 'r') { // remove n
			ret = sscanf(line, "r %zu", &remove_size);
			if (ret == 1) {
				circularBuffer_remove_records(&buf, remove_size);
			}
		} else if (line[0] == 'l') { // pop lifo
			free(peekData);
			peekData = malloc(data_size);
			ret = circularBuffer_popLIFO(&buf, peekData, memcpy);
		} else if (line[0] == 'I') { // pop lifo N
			ret = sscanf(line, "I %zu", &pop_lifo_size);
			if (ret == 1) {
				if (pop_lifo_size > 1 << 16) continue;
				free(peekData);
				peekData = malloc(data_size * pop_lifo_size);
				ret = circularBuffer_popLIFO_n(&buf, peekData, pop_lifo_size, memcpy);
			}
		}
	}
	free(pushData);
	free(peekData);
	free(pushNData);
	free_buffer(&buf);
	free(line);
	return 0;
}
