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
  return rbuff->capacity - rbuff_size(rbuff) - 1;
}
//------------------------------------------------------------------------------
uint32_t rbuff_size(rbuff_t *rbuff)
{
  if(rbuff->write >= rbuff->read)
    return(rbuff->write - rbuff->read);
  else
    return((rbuff->write - rbuff->buff) + (rbuff->buff_end - rbuff->read)) + 1;
}
//------------------------------------------------------------------------------
uint32_t rbuff_write(rbuff_t *rbuff, const uint8_t *buff, uint32_t len)
{
  uint32_t to_end;

  if(rbuff_available(rbuff) == len+1)
    rbuff_size(rbuff); //vejde
  if(rbuff_available(rbuff) == len+0)
    rbuff_size(rbuff); //vejde
  if(rbuff_available(rbuff) == len-1)
    rbuff_size(rbuff); //nevejde
  if(rbuff_available(rbuff) == len-2)
    rbuff_size(rbuff); //nevejde
  if(rbuff_available(rbuff) < len) return 0;

  to_end = rbuff->buff_end - rbuff->write + 1;
  if(to_end == len)
  {
    memcpy(rbuff->write, buff, len);
    rbuff->write = rbuff->buff;
  }
  else if(to_end > len)
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

  return len;
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

  return len;
}
//------------------------------------------------------------------------------
uint8_t rbuff_write_b(rbuff_t *rbuff, uint8_t data)
{
  if(rbuff_available(rbuff) == 1+1)
    rbuff_size(rbuff); //vejde
  if(rbuff_available(rbuff) == 1+0)
    rbuff_size(rbuff); //vejde
  if(rbuff_available(rbuff) == 1-1)
    rbuff_size(rbuff); //nevejde
  if(rbuff_available(rbuff) < 1) return 0;

  if(rbuff->buff_end == rbuff->write)
  {
    *rbuff->write = data;
    rbuff->write = rbuff->buff;
  }
  else if(rbuff->buff_end > rbuff->write)
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

  if(rbuff_available(rbuff) == len+1)
    rbuff_size(rbuff);
  if(rbuff_available(rbuff) == len+0)
    rbuff_size(rbuff);
  if(rbuff_available(rbuff) == len-1)
    rbuff_size(rbuff);
  if(rbuff_available(rbuff) == len-2)
    rbuff_size(rbuff); //nevejde
  if(rbuff_available(rbuff) < len) return 0;

  to_end = rbuff->buff_end - rbuff->write + 1;
  if(to_end == len)
  {
    rbuff_read(w_rbuff, rbuff->write, len);
    rbuff->write = rbuff->buff;
  }
  else if(to_end > len)
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

  return len;
}
//------------------------------------------------------------------------------
uint32_t rbuff_read(rbuff_t *rbuff, uint8_t *buff, uint32_t len)
{
  uint32_t to_end, size;

  size = rbuff_size(rbuff);

  if(size == 0) return 0;

  if(size < len) len = size;

  to_end = rbuff->buff_end - rbuff->read + 1;
  if(to_end == len)
  {
    memcpy(buff, rbuff->read, len);
    rbuff->read = rbuff->buff;
  }
  else if(to_end > len)
  {
    memcpy(buff, rbuff->read, len);
    rbuff->read += len;
  }
  else
  {
    if (to_end==0)
      to_end=0; //to asi nemuze kvuli +1
    if (to_end==1)
      to_end=1;
    if (to_end==len)
      to_end=len;
    memcpy(buff, rbuff->read, to_end);
    memcpy(buff + to_end, rbuff->buff, len - to_end);
    rbuff->read = rbuff->buff + len - to_end;
  }

  return len;
}
//------------------------------------------------------------------------------
uint32_t rbuff_peek(rbuff_t *rbuff, uint8_t *buff, uint32_t len)
{
  uint8_t *proc_jednoduse=rbuff->read;
	uint32_t actual = rbuff_read(rbuff, buff, len);
	rbuff->read=proc_jednoduse;
	/*if (rbuff->read < rbuff->buff + actual)
		rbuff->read += rbuff->buff_end - rbuff->buff - actual;
	else
		rbuff->read -= actual;*/
  return actual;
}
//------------------------------------------------------------------------------
uint8_t rbuff_read_b(rbuff_t *rbuff, uint8_t *data)
{
  if(rbuff_size(rbuff) == 0) return 0;

  if(rbuff->buff_end == rbuff->read)
    rbuff_size(rbuff);
  if(rbuff->buff_end == rbuff->read+1)
    rbuff_size(rbuff);
  if(rbuff->buff_end == rbuff->read)
  {
    *data = *rbuff->read;
    rbuff->read = rbuff->buff;
  }
  else if(rbuff->buff_end > rbuff->read)
  {
    *data = *rbuff->read;
    rbuff->read++;
  }
  else
  { //sem se asi nechodi?
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

  to_end = rbuff->buff_end - rbuff->read + 1;
  if(to_end > len)
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
uint32_t rbuff_seek_wp(rbuff_t *rbuff, int32_t len)
{

}
//------------------------------------------------------------------------------
uint32_t rbuff_set_wp(rbuff_t *rbuff, int32_t pos)
{

}
//------------------------------------------------------------------------------
