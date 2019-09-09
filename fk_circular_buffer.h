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

/*! @mainpage Lager Circular Buffer
 *
 * [TOC]
 *
 * @section intro_sec Introduction
 *
 * This is a module for turning a preallocated array into a circular buffer,
 * suitable for use on embedded systems (no dynamic allocations are
 * performed; a caller-supplied `memcpy` is used; no dependencies other
 * than a C89 compiler and standard library) Data is never overwritten;
 * attempting to add data to an already-full buffer will return an error.
 *
 * @section install_sec Installation
 * Simply copy fk_circular_buffer.c and fk_circular_buffer.h into your project: https://github.com/fictivekin/fk_c_circular_buffer
 *
 */

/**
 * @file fk_circular_buffer.h
 * @author Akbar Dhanaliwala
 * @version 1
 * @date 3 Sep 2019
 * @brief Module to turn static array into circular buffer
 * @details Copyright (c) 2019, Fictive Kin, LLC<br>
 * All rights reserved. <br>
 *
 */

#ifndef _CIRCULARBUFFER_INCLUDED
#define _CIRCULARBUFFER_INCLUDED
/*-------------------------MODULES USED-------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*-------------------------DEFINITIONS AND MACROS---------------------------*/
/** No error. */
#define CIRC_BUF_NO_ERROR 0
/** Insufficient space in buffer to add item(s) */
#define CIRC_BUF_BUFFER_FULL -1
/** Attemped to pop/remove/peek from an empty buffer */
#define CIRC_BUF_BUFFER_EMPTY -2
/** Passed a NULL pointer */
#define CIRC_BUF_ADDR_ERROR -3
/** Invalid size used (too large or too small)  */
#define CIRC_BUF_SIZE_ERROR -4

/*-------------------------TYPEDEFS AND STRUCTURES--------------------------*/

/** Circular buffer */
typedef struct circularBuffer{
	size_t data_size;  /**< Size of an individual element */
	size_t buffer_slots; /**< Number of slots */
	size_t start; /**< Start index */
	size_t end; /**< End index */
	size_t count; /**< Elements in use */
	uint8_t *p_data_location;  /**< data pointer */
} circularBuffer_t;

#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 199901L
   /*C99*/
	#define FK_CB_KW_RESTRICT restrict
#endif
#endif

#ifndef FK_CB_KW_RESTRICT
#define FK_CB_KW_RESTRICT
#endif

/** pointer to a function with the same signature as memcpy */
typedef void *(* memcpy_t)(void * FK_CB_KW_RESTRICT dst, const void * FK_CB_KW_RESTRICT src, size_t num);
/*-------------------------EXPORTED VARIABLES ------------------------------*/
#ifndef _CIRCULARBUFFER_C_SRC

#endif

/*-------------------------EXPORTED FUNCTIONS-------------------------------*/
/**
 * Initialize a circular buffer
 *
 * @param[in] p_buffer pointer to the circular buffer to initialize
 * @param[in] p_data_buffer pointer to the start of the memory location where the
 *						buffer's data will be stored
 * @param[in] data_buffer_size size of \p p_data_buffer in bytes
 * @param[in] item_size item size. \p data_buffer_size must be evenly divisible by
 *								\p item_size
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_SIZE_ERROR if \p data_buffer_size or \p item_size is 0, or
 *								if \p item_size does not evenly divide
 *								\p data_buffer_size
 * @retval CIRC_BUF_NO_ERROR on success
 *
 ******************************************************************************/
int circularBuffer_init(circularBuffer_t *p_buffer, void * p_data_buffer, size_t data_buffer_size, size_t item_size);

/**
 * Flush (empty) a circular buffer
 *
 * @param[in] p_buffer pointer to the circular buffer to flush
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_NO_ERROR on success
 *
 ******************************************************************************/
int circularBuffer_flush(circularBuffer_t *p_buffer);

/**
 * Determine if a circular buffer is full
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @return Whether the buffer is full
 *
 ******************************************************************************/
bool circularBuffer_is_full(const circularBuffer_t * p_buffer);

/**
 * Determine if a circular buffer is empty
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @return Whether the buffer is empty
 *
 ******************************************************************************/
bool circularBuffer_is_empty(const circularBuffer_t *p_buffer);

/**
 * Get number of items in a buffer
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @param[out] result number of items in \p p_buffer
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer or \p result is `NULL`
 * @retval CIRC_BUF_NO_ERROR on success
 ******************************************************************************/
int circularBuffer_getCount(const circularBuffer_t *p_buffer, size_t *result);

/**
 * Push 1 item from \p p_data onto the end of \p p_buffer.
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @param[in] p_data pointer to the data to push onto the buffer. Must be at
 	least \p p_buffer->item_size bytes in length. Must not overlap with
 	\p p_buffer->p_data_location
 * @param[in] fp_memcpy pointer to the function to use to copy memory. If `NULL` is
 	passed, `memcpy` will be used.
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_BUFFER_FULL if \p p_buffer is full
 * @retval CIRC_BUF_NO_ERROR on success
 ******************************************************************************/
int circularBuffer_push(circularBuffer_t *p_buffer, const void * FK_CB_KW_RESTRICT p_data, memcpy_t fp_memcpy);

/**
 * Push \p n items from \p p_data onto the end of \p p_buffer
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @param[in] p_data pointer to the data to push onto the buffer. Must be at
 	least \p p_buffer->item_size * \p n bytes in length. Must not overlap with
 	\p p_buffer->p_data_location.
 * @param[in] n number of items to push onto \p p_buffer
 * @param[in] fp_memcpy pointer to the function to use to copy memory. If `NULL` is
 	passed, `memcpy` will be used.
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_SIZE_ERROR if \p n exceeds \p p_buffer->buffer_slots
 * @retval CIRC_BUF_BUFFER_FULL if \p p_buffer cannot accept \p n more items
 * @retval CIRC_BUF_NO_ERROR on success
 ******************************************************************************/
int circularBuffer_push_n(circularBuffer_t *p_buffer, const void * FK_CB_KW_RESTRICT p_data, size_t n, memcpy_t fp_memcpy);

/**
 * Copy the first \p n items from \p p_buffer into \p p_data
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @param[out] p_data pointer to the destination. Must be at
 	least \p p_buffer->item_size * \p n bytes in length. Must not overlap with
 	\p p_buffer->p_data_location.
 * @param[in] n number of items to read from \p p_buffer
 * @param[in] fp_memcpy pointer to the function to use to copy memory. If `NULL` is
 	passed, `memcpy` will be used.
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_BUFFER_EMPTY if \p p_buffer is empty
 * @retval CIRC_BUF_SIZE_ERROR if \p n is zero or \p n exceeds buffer item count
 * @retval CIRC_BUF_NO_ERROR on success
 ******************************************************************************/
int circularBuffer_peek(const circularBuffer_t *p_buffer, void * FK_CB_KW_RESTRICT p_data, size_t n, memcpy_t fp_memcpy);

/**
 * Copy one item from the beginning of \p p_buffer into \p p_data and remove it from \p p_buffer
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @param[out] p_data pointer to the destination. Must be at
 	least \p p_buffer->item_size bytes in length. Must not overlap with
 	\p p_buffer->p_data_location.
 * @param[in] fp_memcpy pointer to the function to use to copy memory. If `NULL` is
 	passed, `memcpy` will be used.
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_BUFFER_EMPTY if \p p_buffer is empty
 * @retval CIRC_BUF_NO_ERROR on success
 ******************************************************************************/
int circularBuffer_popFIFO(circularBuffer_t *p_buffer, void * FK_CB_KW_RESTRICT p_data, memcpy_t fp_memcpy);

/**
 * Copy \p n items from the beginning of \p p_buffer into \p p_data and remove them
 * from \p p_buffer. If \p n exceeds the size of \p p_buffer, `circularBuffer_popFIFO_n`
 * will pop all the elements from \p p_buffer.
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @param[out] p_data pointer to the destination. Must be at
 	least \p p_buffer->item_size * \p n bytes in length. Must not overlap with
 	\p p_buffer->p_data_location.
 * @param[in] n number of items to pop
 * @param[in] fp_memcpy pointer to the function to use to copy memory. If `NULL` is
 	passed, `memcpy` will be used.
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_BUFFER_EMPTY if \p p_buffer is empty
 * @retval CIRC_BUF_SIZE_ERROR if \p n is zero
 * @retval CIRC_BUF_NO_ERROR on success
 ******************************************************************************/
int circularBuffer_popFIFO_n(circularBuffer_t *p_buffer, void * FK_CB_KW_RESTRICT p_data, size_t n, memcpy_t fp_memcpy);

/**
 * Copy one item from the end of \p p_buffer into \p p_data and remove it from \p p_buffer
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @param[out] p_data pointer to the destination. Must be at
 	least \p p_buffer->item_size bytes in length. Must not overlap with
 	\p p_buffer->p_data_location.
 * @param[in] fp_memcpy pointer to the function to use to copy memory. If `NULL` is
 	passed, `memcpy` will be used.
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_BUFFER_EMPTY if \p p_buffer is empty
 * @retval CIRC_BUF_NO_ERROR on success
 ******************************************************************************/
int circularBuffer_popLIFO(circularBuffer_t *p_buffer, void * FK_CB_KW_RESTRICT p_data, memcpy_t fp_memcpy);

/**
 * Copy \p n items from the end of \p p_buffer into \p p_data and remove them
 * from \p p_buffer. If \p n exceeds the size of \p p_buffer, `circularBuffer_popFIFO_n`
 * will pop all the elements from \p p_buffer.
 *
 * @param[in] p_buffer pointer to the circular buffer
 * @param[out] p_data pointer to the destination. Must be at
 	least \p p_buffer->item_size * \p n bytes in length. Must not overlap with
 	\p p_buffer->p_data_location.
 * @param[in] n number of items to pop
 * @param[in] fp_memcpy pointer to the function to use to copy memory. If `NULL` is
 	passed, `memcpy` will be used.
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_BUFFER_EMPTY if \p p_buffer is empty
 * @retval CIRC_BUF_SIZE_ERROR if \p n is zero
 * @retval CIRC_BUF_NO_ERROR on success
 ******************************************************************************/
int circularBuffer_popLIFO_n(circularBuffer_t *p_buffer, void * FK_CB_KW_RESTRICT p_data, size_t n, memcpy_t fp_memcpy);

/**
 * Remove \p n items from the beginning of \p p_buffer. If \p n exceeds the number of items in
 * \p p_buffer, all of the items will be removed.
 *
 * @param[in] p_buffer pointer to circular buffer
 * @param[in] n number of items to remove
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer is `NULL`
 * @retval CIRC_BUF_BUFFER_EMPTY if \p p_buffer is empty
 * @retval CIRC_BUF_NO_ERROR on success
 *
 ******************************************************************************/
int circularBuffer_remove_records(circularBuffer_t *p_buffer, size_t n);

/**
 * Get number of slots in a circular buffer
 *
 * @param[in] p_buffer pointer to circular buffer
 * @param[out] result number of slots in \p p_buffer
 * @retval CIRC_BUF_ADDR_ERROR if \p p_buffer or \p result is `NULL`
 * @retval CIRC_BUF_NO_ERROR on success
 *
 ******************************************************************************/
int circularBuffer_max_slots(const circularBuffer_t *p_buffer, size_t *result);

/**
 * Copy \p src buffer into \p dst. \p src and \p dst must both have been
 * initialized with circularBuffer_init, and \p dst->p_data_location must be
 * at least as large as \p src->p_data_location. \p dst->p_data_location must
 * not overlap \p src->p_data_location
 *
 * @param[out] dst Pointer to destination buffer
 * @param[in] src Pointer to source buffer
 * @param[in] fp_memcpy pointer to the function to use to copy memory. If `NULL` is
 	passed, `memcpy` will be used.
 * @retval CIRC_BUF_ADDR_ERROR if \p dst or \p src is `NULL`
 * @retval CIRC_BUF_SIZE_ERROR if destination buffer is smaller than source buffer
 * @retval CIRC_BUF_NO_ERROR on success
 *
 ******************************************************************************/
int circularBuffer_copy(circularBuffer_t *dst, const circularBuffer_t *src, memcpy_t fp_memcpy);

#endif
/*-------------------------EOF----------------------------------------------*/
