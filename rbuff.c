/*
 * usart.c
 *
 *  Created on: Apr 9, 2011
 *      Author: Martin
 */
//------------------------------------------------------------------------------
#include "rbuff.h"

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
}
//------------------------------------------------------------------------------
void rbuff_lin_space(rbuff_t *rbuff, uint8_t *buff, uint32_t size)
{
  rbuff->buff = buff;
  rbuff->buff_end = buff + size;

  rbuff->read = buff;
  rbuff->write = buff + size;

  rbuff->capacity = size;
}
//------------------------------------------------------------------------------
void rbuff_reset(rbuff_t *rbuff)
{
  rbuff->read = rbuff->write;
}
//------------------------------------------------------------------------------
uint32_t rbuff_cap(rbuff_t *rbuff)
{
  return rbuff->capacity;
}
//------------------------------------------------------------------------------
uint32_t rbuff_available(rbuff_t *rbuff)
{
  return rbuff->capacity - rbuff_size(rbuff);
}
//------------------------------------------------------------------------------
uint32_t rbuff_size(rbuff_t *rbuff)
{
  if(rbuff->write >= rbuff->read)
    return(rbuff->write - rbuff->read);
  else
    return((rbuff->write - rbuff->buff) + (rbuff->buff_end - rbuff->read));
}
//------------------------------------------------------------------------------
uint32_t rbuff_write(rbuff_t *rbuff, uint8_t *buff, uint32_t len)
{
  uint32_t to_end;

  if(rbuff->capacity - rbuff_size(rbuff) < len) return 0;

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

  return 1;
}
//------------------------------------------------------------------------------
uint32_t rbuff_write_force(rbuff_t *rbuff, uint8_t *buff, uint32_t len)
{
  uint32_t to_end;

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

  return 1;
}
//------------------------------------------------------------------------------
uint8_t rbuff_write_b(rbuff_t *rbuff, uint8_t data)
{
  if(rbuff->capacity - rbuff_size(rbuff) < 1) return 0;
  
  if(rbuff->buff_end > rbuff->write)
  {
    *rbuff->write = data;
    rbuff->write++;
  }
  else
  {
    rbuff->write = rbuff->buff;
    *rbuff->write = data;
    rbuff->write++;
  }

  return 1;
}
//------------------------------------------------------------------------------
uint8_t rbuff_write_b_force(rbuff_t *rbuff, uint8_t data)
{
  if(rbuff->buff_end > rbuff->write)
  {
    *rbuff->write = data;
    rbuff->write++;
  }
  else
  {
    rbuff->write = rbuff->buff;
    *rbuff->write = data;
    rbuff->write++;
  }

  return 1;
}
//------------------------------------------------------------------------------
uint8_t rbuff_write_rb(rbuff_t *rbuff, rbuff_t *w_rbuff, uint32_t len)
{
  uint32_t to_end;

  if(rbuff->capacity - rbuff_size(rbuff) < len) return 0;

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

  return 1;
}
//------------------------------------------------------------------------------
uint32_t rbuff_read(rbuff_t *rbuff, uint8_t *buff, uint32_t len)
{
  uint32_t to_end, size;

  size = rbuff_size(rbuff);

  if(size == 0) return 0;

  if(size < len) len = size;

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

  return len;
}
//------------------------------------------------------------------------------
uint8_t rbuff_read_b(rbuff_t *rbuff, uint8_t *data)
{
  if(rbuff_size(rbuff) == 0) return 0;

  if(rbuff->buff_end > rbuff->read)
  {
    *data = *rbuff->read;
    rbuff->read++;
  }
  else
  {
    rbuff->read = rbuff->buff;
    *data = *rbuff->read;
    rbuff->read++;
  }

  return 1;
}
//------------------------------------------------------------------------------
uint32_t rbuff_seek(rbuff_t *rbuff, int32_t len)
{
  uint32_t to_end, size;

  size = rbuff_size(rbuff);

  if(size == 0) return 0;
  if(size < len) len = size;

  to_end = rbuff->buff_end - rbuff->read;
  if(to_end >= len)
  {
    rbuff->read += len;
  }
  else
  {
    rbuff->read = rbuff->buff + len - to_end;
  }

  return len;
}
//------------------------------------------------------------------------------
