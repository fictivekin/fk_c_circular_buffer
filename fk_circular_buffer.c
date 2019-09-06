/****************************************************************************
 * Copyright (C) 2019 by Fictive Kin                                        *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files                 *
 * (the "Software"), to deal in the Software without restriction, including *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, sublicense, and/or sell copies of the Software, and to       *
 * permit persons to whom the Software is furnished to do so, subject to    *
 * the following conditions:                                                *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     *
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     *
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   *
 ****************************************************************************/

/**
 * @file fk_circular_buffer.c
 * @author Akbar Dhanaliwala
 * @version 1
 * @date 3 Sep 2019
 * @brief Module to turn static array into circular buffer
 * @details Copyright (c) 2019, Fictive Kin, LLC<br>
 * All rights reserved. <br>
*/

#define _CIRCULARBUFFER_C_SRC

/*-------------------------MODULES USED-------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fk_circular_buffer.h"
/*-------------------------DEFINITIONS AND MACORS---------------------------*/

#define VERIFY_ADDR(addr) {if(NULL==addr){return CIRC_BUF_ADDR_ERROR;}}
#define VERIFY_SIZE(size) {if(0==size){return CIRC_BUF_SIZE_ERROR;}}
/*-------------------------TYPEDEFS AND STRUCTURES--------------------------*/



/*-------------------------PROTOTYPES OF LOCAL FUNCTIONS--------------------*/



/*-------------------------EXPORTED VARIABLES ------------------------------*/



/*-------------------------GLOBAL VARIABLES---------------------------------*/

/*-------------------------EXPORTED FUNCTIONS-------------------------------*/
int circularBuffer_init(circularBuffer_t *p_buffer, void *p_data_buffer, size_t data_buffer_size, size_t item_size)
{
	VERIFY_ADDR(p_buffer);
	VERIFY_SIZE(data_buffer_size);
	VERIFY_SIZE(item_size);

	if (data_buffer_size % item_size != 0) {
		return CIRC_BUF_SIZE_ERROR;
	}

	p_buffer->start = 0;
	p_buffer->count = 0;
	p_buffer->end = 0;
	p_buffer->buffer_slots = data_buffer_size / item_size;
	p_buffer->data_size = item_size;
	p_buffer->p_data_location = p_data_buffer;

    return CIRC_BUF_NO_ERROR;
}

int circularBuffer_flush(circularBuffer_t *p_buffer)
{
	VERIFY_ADDR(p_buffer);
	p_buffer->start = 0;
	p_buffer->count = 0;
	p_buffer->end = 0;

	return CIRC_BUF_NO_ERROR;
}

bool circularBuffer_is_full(const circularBuffer_t *p_buffer)
{
	if (p_buffer == NULL) return true;
	return p_buffer->count == p_buffer->buffer_slots;
}

bool circularBuffer_is_empty(const circularBuffer_t *p_buffer)
{
	if (p_buffer == NULL) return true;

	return 0 == p_buffer->count;
}

int circularBuffer_getCount(const circularBuffer_t *p_buffer, size_t *result)
{
	VERIFY_ADDR(p_buffer);
	VERIFY_ADDR(result);

	*result = p_buffer->count;
	return CIRC_BUF_NO_ERROR;
}

int circularBuffer_push(circularBuffer_t *p_buffer, const void * FK_CB_KW_RESTRICT p_data, memcpy_t fp_memcpy)
{
	VERIFY_ADDR(p_buffer);
	fp_memcpy = fp_memcpy ? fp_memcpy : memcpy;

	if(p_buffer->count > p_buffer->buffer_slots - 1) {
		return CIRC_BUF_BUFFER_FULL;
	}

	fp_memcpy((uint8_t *)(p_buffer->p_data_location) + (p_buffer->end)*p_buffer->data_size, p_data, p_buffer->data_size);

	p_buffer->count++;
	p_buffer->end++;
	if(p_buffer->end >= p_buffer->buffer_slots) {
		p_buffer->end = 0;
	}

	return CIRC_BUF_NO_ERROR;
}


int circularBuffer_push_n(circularBuffer_t *p_buffer, const void * FK_CB_KW_RESTRICT p_data, size_t n, memcpy_t fp_memcpy)
{
	size_t bytes_to_copy;
	size_t bytes_copied;

	VERIFY_ADDR(p_buffer);
	VERIFY_SIZE(n);
	fp_memcpy = fp_memcpy ? fp_memcpy : memcpy;

	if (n > p_buffer->buffer_slots) {
		return CIRC_BUF_SIZE_ERROR;
	}

	if(p_buffer->count > p_buffer->buffer_slots - n) {
		return CIRC_BUF_BUFFER_FULL;
	}

	bytes_to_copy = n * p_buffer->data_size;
	if (p_buffer->end + n > p_buffer->buffer_slots) {
		/* we're gonna overflow */
		bytes_copied = (p_buffer->buffer_slots - p_buffer->end) * p_buffer->data_size;
		fp_memcpy(
			(uint8_t *)(p_buffer->p_data_location) + (p_buffer->end * p_buffer->data_size),
			p_data,
			bytes_copied
		);

		fp_memcpy(
			p_buffer->p_data_location,
			(uint8_t *)p_data + bytes_copied,
			bytes_to_copy - bytes_copied
		);
	} else {
		fp_memcpy(
			(uint8_t *)(p_buffer->p_data_location) + (p_buffer->end * p_buffer->data_size),
			p_data,
			bytes_to_copy
		);
	}

	p_buffer->count += n;
	p_buffer->end = (p_buffer->end + n) % p_buffer->buffer_slots;

	return CIRC_BUF_NO_ERROR;
}


int circularBuffer_peek(const circularBuffer_t *p_buffer, void * FK_CB_KW_RESTRICT p_data, size_t n, memcpy_t fp_memcpy)
{
	size_t bytes_copied = 0;
	size_t bytes_to_copy;
	VERIFY_ADDR(p_buffer);
	VERIFY_SIZE(n);
	fp_memcpy = fp_memcpy ? fp_memcpy : memcpy;

	if (0 == p_buffer->count) {
		return CIRC_BUF_BUFFER_EMPTY;
	}
	if (n > p_buffer->count) {
		return CIRC_BUF_SIZE_ERROR;
	}

	bytes_to_copy = n * p_buffer->data_size;
	if (p_buffer->start > p_buffer->buffer_slots - n) {
		/* we're gonna overflow*/
		bytes_copied = (p_buffer->buffer_slots - p_buffer->start) * p_buffer->data_size;
		fp_memcpy(
			p_data,
			(uint8_t *)(p_buffer->p_data_location) + (p_buffer->start)*p_buffer->data_size,
			bytes_copied
		);
		fp_memcpy(
			(uint8_t *)p_data + bytes_copied,
			p_buffer->p_data_location,
			bytes_to_copy - bytes_copied
		);
	} else {
		fp_memcpy(
			p_data,
			(uint8_t *)(p_buffer->p_data_location) + (p_buffer->start)*p_buffer->data_size,
			bytes_to_copy
		);
	}

	return CIRC_BUF_NO_ERROR;
}

int circularBuffer_popFIFO(circularBuffer_t * p_buffer, void * FK_CB_KW_RESTRICT p_data, memcpy_t fp_memcpy)
{
	VERIFY_ADDR(p_buffer);
	fp_memcpy = fp_memcpy ? fp_memcpy : memcpy;

	if(0 == p_buffer->count) {
		return CIRC_BUF_BUFFER_EMPTY;
	}

	circularBuffer_peek(p_buffer, p_data, 1, fp_memcpy);

	p_buffer->count--;
	p_buffer->start++;
	if(p_buffer->start >= p_buffer->buffer_slots) {
		p_buffer->start = 0;
	}

	return CIRC_BUF_NO_ERROR;
}

int circularBuffer_popFIFO_n(circularBuffer_t *p_buffer, void * FK_CB_KW_RESTRICT p_data, size_t n, memcpy_t fp_memcpy)
{
	VERIFY_ADDR(p_buffer);
	fp_memcpy = fp_memcpy ? fp_memcpy : memcpy;

	if(n > p_buffer->count) {
		n = p_buffer->count;
	}
	if (0 == n) {
		return CIRC_BUF_BUFFER_EMPTY;
	}

	circularBuffer_peek(p_buffer, p_data, n, fp_memcpy);

	p_buffer->count -= n;
	p_buffer->start = (p_buffer->start + n) % p_buffer->buffer_slots;

	return CIRC_BUF_NO_ERROR;
}

int circularBuffer_remove_records(circularBuffer_t *p_buffer, size_t n)
{
	VERIFY_ADDR(p_buffer);

	if (0 == p_buffer->count) {
		return CIRC_BUF_BUFFER_EMPTY;
	}

	if (n > p_buffer->count) {
		n = p_buffer->count;
	}

	p_buffer->count -= n;
	p_buffer->start = (p_buffer->start + n) % p_buffer->buffer_slots;

	return CIRC_BUF_NO_ERROR;
}

int circularBuffer_popLIFO(circularBuffer_t * p_buffer, void * FK_CB_KW_RESTRICT p_data, memcpy_t fp_memcpy)
{
	VERIFY_ADDR(p_buffer);
	fp_memcpy = fp_memcpy ? fp_memcpy : memcpy;

	if (0 == p_buffer->count) {
		return CIRC_BUF_BUFFER_EMPTY;
	}

	if (0 == p_buffer->end) {
		p_buffer->end = p_buffer->buffer_slots - 1;
	} else{
		p_buffer->end--;
	}
	fp_memcpy(
		p_data,
		(uint8_t *)p_buffer->p_data_location + (p_buffer->end)*p_buffer->data_size,
		p_buffer->data_size
	);
	p_buffer->count--;

	return CIRC_BUF_NO_ERROR;
}

int circularBuffer_popLIFO_n(circularBuffer_t *p_buffer, void * FK_CB_KW_RESTRICT p_data, size_t n, memcpy_t fp_memcpy)
{
	size_t bytes_to_copy;
	size_t bytes_copied;
	size_t start_offset;
	VERIFY_ADDR(p_buffer);
	fp_memcpy = fp_memcpy ? fp_memcpy : memcpy;

	if (0 == p_buffer->count) {
		return CIRC_BUF_BUFFER_EMPTY;
	}
	if(n > p_buffer->count) {
		n = p_buffer->count;
	}

	if (p_buffer->end < n) {
		start_offset = (p_buffer->start + (p_buffer->buffer_slots - n)) % p_buffer->buffer_slots;
	} else {
		start_offset = p_buffer->end - n;
	}

	bytes_to_copy = n * p_buffer->data_size;
	if (start_offset > p_buffer->buffer_slots - n) {
		bytes_copied = (p_buffer->buffer_slots - start_offset) * p_buffer->data_size;
		fp_memcpy(
			p_data,
			(uint8_t *)(p_buffer->p_data_location) + start_offset*p_buffer->data_size,
			bytes_copied
		);
		fp_memcpy(
			(uint8_t *)p_data + bytes_copied,
			p_buffer->p_data_location,
			bytes_to_copy - bytes_copied
		);
	} else {
		fp_memcpy(
			p_data,
			(uint8_t *)(p_buffer->p_data_location) + start_offset * p_buffer->data_size,
			bytes_to_copy
		);
	}

	p_buffer->count -= n;
	p_buffer->end = (p_buffer->start + p_buffer->count) % p_buffer->buffer_slots;

	return CIRC_BUF_NO_ERROR;
}


int circularBuffer_max_slots(const circularBuffer_t *p_buffer, size_t *result)
{
	VERIFY_ADDR(p_buffer);
	VERIFY_ADDR(result);
	*result = p_buffer->buffer_slots;
	return CIRC_BUF_NO_ERROR;
}

int circularBuffer_copy(circularBuffer_t *dst, const circularBuffer_t *src, memcpy_t fp_memcpy)
{
	size_t dst_buffer_size;
	size_t src_buffer_size;
	VERIFY_ADDR(dst);
	VERIFY_ADDR(src);
	fp_memcpy = fp_memcpy ? fp_memcpy : memcpy;

	dst_buffer_size = dst->buffer_slots * dst->data_size;
	src_buffer_size = src->buffer_slots * src->data_size;
	if (dst_buffer_size < src_buffer_size) {
		return CIRC_BUF_SIZE_ERROR;
	}

	dst->data_size = src->data_size;
	dst->buffer_slots = src->buffer_slots;
	dst->start = src->start;
	dst->end = src->end;
	dst->count = src->count;
	fp_memcpy(dst->p_data_location, src->p_data_location, src_buffer_size);

	return CIRC_BUF_NO_ERROR;
}
/*-------------------------LOCAL FUNCTIONS-----------------------------------*/



/*-------------------------EOF----------------------------------------------*/
