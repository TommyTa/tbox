/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_flash.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/
#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "typedefs.h"

int32_t  bsp_flash_erase_page(uint32_t addr);

uint8_t  bsp_flash_read_byte(uint32_t addr);
void     bsp_flash_read_bytes(uint32_t addr, uint8_t *data, uint32_t len);
uint32_t bsp_flash_read_word(uint32_t addr);
void     bsp_flash_read_words(uint32_t addr, uint32_t *data, uint32_t len);

int32_t  bsp_flash_write_words(uint32_t addr, uint32_t *data, uint32_t len);
int32_t  bsp_flash_carry(uint32_t t_addr, uint32_t s_addr, uint32_t size);

void     bsp_flash_erase_nv(void);
int32_t  bsp_flash_write_nv(uint16_t data);
uint16_t bsp_flash_read_nv(void);

#endif
