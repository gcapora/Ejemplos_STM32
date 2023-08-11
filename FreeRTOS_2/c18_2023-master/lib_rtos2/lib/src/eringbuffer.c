/*
 * eringbuffer.c
 *
 *  Created on: Feb 15, 2023
 *      Author: seb
 */

#include "eringbuffer.h"

inline static void inc_idex_(size_t *index, size_t size)
{
  *index += 1;
  if(size <= *index)
  {
    *index = 0;
  }
}

inline static void write_byte_(eringbuffer_t *rb, uint8_t byte)
{
  rb->buffer[rb->w] = byte;
  inc_idex_(&(rb->w), rb->size);
  rb->len++;
}

inline static void read_byte_(eringbuffer_t *rb, uint8_t *byte)
{
  *byte = rb->buffer[rb->r];
  inc_idex_(&(rb->r), rb->size);
  rb->len--;
}

void eringbuffer_init(eringbuffer_t *rb, uint8_t *buffer, size_t size)
{
  rb->buffer = buffer;
  rb->size = size;
  rb->len = 0;
  rb->w = 0;
  rb->r = 0;
}

size_t eringbuffer_size(const eringbuffer_t *rb)
{
  return rb->size;
}

size_t eringbuffer_len(const eringbuffer_t *rb)
{
  return rb->len;
}

size_t eringbuffer_free(const eringbuffer_t *rb)
{
  return rb->size - rb->len;
}

bool eringbuffer_is_full(const eringbuffer_t *rb)
{
  return (0 == eringbuffer_free(rb));
}

bool eringbuffer_is_empty(const eringbuffer_t *rb)
{
  return (0 == eringbuffer_len(rb));
}

size_t eringbuffer_write_byte(eringbuffer_t *rb, uint8_t byte)
{
  if(eringbuffer_is_full(rb))
  {
    return 0;
  }
  write_byte_(rb, byte);
  return 1;
}

size_t eringbuffer_write(eringbuffer_t *rb, const uint8_t *buffer, size_t size)
{
  size_t i;
  for(i = 0; i < size; ++i)
  {
    if(0 == eringbuffer_write_byte(rb, buffer[i]))
    {
      break;
    }
  }
  return i;
}

size_t eringbuffer_read_byte(eringbuffer_t *rb, uint8_t *byte)
{
  if(eringbuffer_is_empty(rb))
  {
    return 0;
  }
  read_byte_(rb, byte);
  return 1;
}

size_t eringbuffer_read(eringbuffer_t *rb, uint8_t *buffer, size_t size)
{
  size_t i;
  for(i = 0; i < size; ++i)
  {
    if(0 == eringbuffer_read_byte(rb, buffer + i))
    {
      break;
    }
  }
  return i;
}
