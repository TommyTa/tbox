/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_i2c.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/
 #ifndef __BSP_I2C_H__
 #define __BSP_I2C_H__

#include <stdint.h>

#define BSP_I2C_MULTI
typedef enum {
    BSP_I2C_BUS1,
#ifdef BSP_I2C_MULTI
    BSP_I2C_BUS2,
    BSP_I2C_BUS3,
#endif
    BSP_I2C_BUS_NUM,
} bsp_i2c_bus_t;

typedef struct {
    bsp_i2c_bus_t bus;
    uint8_t slave_addr;
} i2c_transfer_t;

void bsp_i2c_init(bsp_i2c_bus_t bus);
int32_t bsp_i2c_write(i2c_transfer_t* i2c, uint8_t reg, uint8_t* data, uint16_t len);
int32_t bsp_i2c_read(i2c_transfer_t* i2c, uint8_t reg, uint8_t* data, uint16_t len);

#endif // __BSP_I2C_H__
