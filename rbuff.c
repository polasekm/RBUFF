/*
  * rbuff.c
 *
 *  Created on: 1.6. 2011
 *      Author: Martin Polasek
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
static uint32_t rbuff_size_total=0;
static uint32_t rbuff_size_diff=0;
uint32_t rbuff_size(rbuff_t *rbuff)
{
  if(rbuff->write >= rbuff->read)
    return(rbuff->write - rbuff->read);
  else
  {
    return rbuff->write - rbuff->read + rbuff->capacity;
    ////end=buff+cap-1 -> rbuff->write - rbuff->read + cap
    uint32_t x1=((rbuff->write - rbuff->buff) + (rbuff->buff_end - rbuff->read)) + 1;
    uint32_t x2=rbuff->write - rbuff->read + rbuff->capacity;
    rbuff_size_total++;
    if (x1!=x2)
      rbuff_size_diff++; //se deje protoze mezi tim pribyvaji data
    //a test je na prd, kdyz dam x2+1 tak da access violation, x2-1 bezi na veky
    //kdyz pocitam x2 stejne jako x1 tak chyby rostou zhruba stejne
    return x2;
  }
}
//------------------------------------------------------------------------------
uint32_t rbuff_write(rbuff_t *rbuff, const uint8_t *buff, uint32_t len)
{
  uint32_t to_end;

  if(rbuff_available(rbuff) < len) return 0;

  to_end = rbuff->buff_end - rbuff->write + 1;
  if(to_end > len)
  {
    memcpy(rbuff->write, buff, len);
    rbuff->write += len;
  }
  else if(to_end == len)
  {
    memcpy(rbuff->write, buff, to_end);
    rbuff->write = rbuff->buff;
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
{ //pravdepodobne nefunguje
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
  if(rbuff_available(rbuff) < 1) return 0;

  if(rbuff->buff_end > rbuff->write)
  {
    *rbuff->write = data;
    rbuff->write++;
  }
  else if(rbuff->buff_end == rbuff->write)
  {
    *rbuff->write = data;
    rbuff->write = rbuff->buff;
  }
  else
  { //sem se asi nechodi
    rbuff->write = rbuff->buff;
    *rbuff->write = data;
    rbuff->write++;
  }

  return 1;
}
//------------------------------------------------------------------------------
uint8_t rbuff_write_b_force(rbuff_t *rbuff, uint8_t data)
{ //pravdepodobne nefunguje
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

  if(rbuff_available(rbuff) < len) return 0;

  to_end = rbuff->buff_end - rbuff->write + 1;
  if(to_end > len)
  {
    rbuff_read(w_rbuff, rbuff->write, len);
    rbuff->write += len;
  }
  else if(to_end == len)
  {
    rbuff_read(w_rbuff, rbuff->write, len);
    rbuff->write = rbuff->buff;
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
  if(to_end > len)
  {
    memcpy(buff, rbuff->read, len);
    rbuff->read += len;
  }
  else if(to_end == len)
  {
    memcpy(buff, rbuff->read, len);
    rbuff->read = rbuff->buff;
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
uint32_t rbuff_peek(rbuff_t *rbuff, uint8_t *buff, uint32_t len)
{
  uint8_t *proc_jednoduse=rbuff->read;
	uint32_t actual = rbuff_read(rbuff, buff, len);
	rbuff->read=proc_jednoduse;
  return actual;
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
  else if(rbuff->buff_end == rbuff->read)
  {
    *data = *rbuff->read;
    rbuff->read = rbuff->buff;
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
/*uint32_t rbuff_seek_wp(rbuff_t *rbuff, int32_t len)
{

}
//------------------------------------------------------------------------------
uint32_t rbuff_set_wp(rbuff_t *rbuff, int32_t pos)
{

}*/
//------------------------------------------------------------------------------
