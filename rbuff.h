/*-----------------------------------------------------------------------------*/
/*
 * rbuff.h
 *
 *  Created on: 1.6. 2011
 *      Author: Martin Polasek
 */
/*-----------------------------------------------------------------------------*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RING_BUFFER_H_INCLUDED
#define RING_BUFFER_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  Ring buffer structure definition
  */
typedef struct
{
  uint8_t *buff;        //ukazatel na pocatek bufferu
  uint8_t *buff_end;    //ukazatel na konec bufferu

  uint8_t *read;        //cteci ukazatel
  uint8_t *write;       //zapisovaci ukazatel

  uint32_t capacity;    //kapacita bufferu

} rbuff_t;

/* Exported functions ------------------------------------------------------- */

/* Defines -------------------------------------------------------------------*/
// Functions -------------------------------------------------------------------
void rbuff_reset(rbuff_t *rbuff);
void rbuff_init(rbuff_t *rbuff, uint8_t *buff, uint32_t size);
void rbuff_lin_space(rbuff_t *rbuff, uint8_t *buff, uint32_t size);

uint32_t rbuff_capacity(rbuff_t *rbuff);
uint32_t rbuff_available(rbuff_t *rbuff);
uint32_t rbuff_size(rbuff_t *rbuff);

uint32_t rbuff_write(rbuff_t *rbuff, const uint8_t *buff, uint32_t len);
uint32_t rbuff_write_force(rbuff_t *rbuff, uint8_t *buff, uint32_t len);
uint8_t  rbuff_write_b(rbuff_t *rbuff, uint8_t data);
uint8_t  rbuff_write_b_force(rbuff_t *rbuff, uint8_t data);
uint8_t rbuff_write_rb(rbuff_t *rbuff, rbuff_t *w_rbuff, uint32_t len);

uint8_t rbuff_copy(rbuff_t *rbuff_dst, rbuff_t *rbuff_src, uint32_t len);
uint8_t rbuff_move(rbuff_t *rbuff_dst, rbuff_t *rbuff_src, uint32_t len);

uint32_t rbuff_read(rbuff_t *rbuff, uint8_t *buff, uint32_t len);
uint8_t  rbuff_read_b(rbuff_t *rbuff, uint8_t *data);

uint32_t rbuff_peek(rbuff_t *rbuff, uint8_t *buff, uint32_t len);
uint32_t rbuff_peek_b(rbuff_t *rbuff, uint8_t *data);

uint32_t rbuff_seek(rbuff_t *rbuff, int32_t len);

uint32_t rbuff_seek_wp(rbuff_t *rbuff, int32_t len);
uint32_t rbuff_set_wp(rbuff_t *rbuff, int32_t pos);
//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif /* RING_BUFFER_H_INCLUDED */
