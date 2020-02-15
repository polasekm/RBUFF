/*
 * usart.c
 *
 *  Created on: Apr 9, 2011
 *      Author: Martin
 */
//------------------------------------------------------------------------------
#include "ring_buffer.h"
#include "cmsis_gcc.h"

//------------------------------------------------------------------------------
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
//------------------------------------------------------------------------------
void rbuff_init(rbuff_t *rbuff, uint8_t *buff, uint32_t size)
{
  rbuff->buff = buff;
  rbuff->buff_end = buff + size - 1;

  rbuff->read = buff;
  rbuff->write = buff;

  rbuff->capacity = size;
  rbuff->size = 0;
}
//------------------------------------------------------------------------------
void rbuff_reset(rbuff_t *rbuff)
{
  rbuff->read = rbuff->write;
  rbuff->size = 0;
}
//------------------------------------------------------------------------------
uint32_t rbuff_cap(rbuff_t *rbuff)
{
  return rbuff->capacity;
}
//------------------------------------------------------------------------------
uint32_t rbuff_available(rbuff_t *rbuff)
{
  return rbuff->capacity - rbuff->size;
}
//------------------------------------------------------------------------------
uint32_t rbuff_size(rbuff_t *rbuff)
{
  return rbuff->size;
}
//------------------------------------------------------------------------------
uint8_t rbuff_write(rbuff_t *rbuff, uint8_t *buff, uint32_t len)
{
  uint32_t to_end;

  if(rbuff->capacity - rbuff->size < len) return 0;

  to_end = rbuff->buff_end - rbuff->write;
  if(to_end >= len)
  {
    memcpy(rbuff->write, buff, len);
    rbuff->write += len;
  }
  else
  {
    memcpy(rbuff->write, buff, to_end);
    memcpy(rbuff->buff, buff + to_end, len - to_end);
    rbuff->write = rbuff->buff + len - to_end;
  }

  __disable_irq();
  rbuff->size += len;
  __enable_irq();

  return 1;
}
//------------------------------------------------------------------------------
uint8_t rbuff_write_rb(rbuff_t *rbuff, rbuff_t *w_rbuff, uint32_t len)
{
  uint32_t to_end;

  if(rbuff->capacity - rbuff->size < len) return 0;

  to_end = rbuff->buff_end - rbuff->write;
  if(to_end >= len)
  {
    rbuff_read(w_rbuff, rbuff->write, len);
    rbuff->write += len;
  }
  else
  {
    rbuff_read(w_rbuff, rbuff->write, to_end);
    rbuff_read(w_rbuff, rbuff->buff, len - to_end);
    rbuff->write = rbuff->buff + len - to_end;
  }

  __disable_irq();
  rbuff->size += len;
  __enable_irq();

  return 1;
}
//------------------------------------------------------------------------------
uint32_t rbuff_read(rbuff_t *rbuff, uint8_t *buff, uint32_t len)
{
  uint32_t to_end;

  if(rbuff->size == 0) return 0;
  if(rbuff->size < len) len = rbuff->size;

  to_end = rbuff->buff_end - rbuff->read;
  if(to_end >= len)
  {
    memcpy(buff, rbuff->read, len);
    rbuff->read += len;
  }
  else
  {
    memcpy(buff, rbuff->read, to_end);
    memcpy(buff + to_end, rbuff->buff, len - to_end);
    rbuff->read = rbuff->buff + len - to_end;
  }

  __disable_irq();
  rbuff->size -= len;
  __enable_irq();

  return len;
}
//------------------------------------------------------------------------------
