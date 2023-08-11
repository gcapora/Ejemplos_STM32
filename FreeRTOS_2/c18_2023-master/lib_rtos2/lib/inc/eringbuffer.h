/*
 * eringbuffer.h
 *
 *  Created on: Feb 15, 2023
 *      Author: seb
 */

#ifndef INC_ERINGBUFFER_H_
#define INC_ERINGBUFFER_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t *buffer;
    size_t size;
    size_t len;
    size_t w;
    size_t r;
} eringbuffer_t;

void eringbuffer_init(eringbuffer_t *rb, uint8_t *buffer, size_t size);

size_t eringbuffer_size(const eringbuffer_t *rb);

size_t eringbuffer_len(const eringbuffer_t *rb);

size_t eringbuffer_free(const eringbuffer_t *rb);

bool eringbuffer_is_full(const eringbuffer_t *rb);

bool eringbuffer_is_empty(const eringbuffer_t *rb);

size_t eringbuffer_write_byte(eringbuffer_t *rb, uint8_t byte);

size_t eringbuffer_write(eringbuffer_t *rb, const uint8_t *buffer, size_t size);

size_t eringbuffer_read_byte(eringbuffer_t *rb, uint8_t *byte);

size_t eringbuffer_read(eringbuffer_t *rb, uint8_t *buffer, size_t size);

#endif /* INC_ERINGBUFFER_H_ */
