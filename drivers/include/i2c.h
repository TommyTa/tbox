/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file i2c.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef __I2C_H__
#define __I2C_H__

#include "typedefs.h"
#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_DEV_FLAG_WR             (0)
#define I2C_DEV_FLAG_RD             (1u << 0)
#define I2C_DEV_FLAG_ADDR_10BIT     (1u << 2)  /* this is a ten bit chip address */
#define I2C_DEV_FLAG_NO_START       (1u << 4)
#define I2C_DEV_FLAG_IGNORE_NACK    (1u << 5)
#define I2C_DEV_FLAG_NO_READ_ACK    (1u << 6)  /* when I2C reading, we do not ACK */

#define I2C_DEV_CTRL_10BIT          0x20
#define I2C_DEV_CTRL_ADDR           0x21
#define I2C_DEV_CTRL_TIMEOUT        0x22
#define I2C_DEV_CTRL_RW             0x23

typedef struct
{
    uint16_t addr;
    uint16_t flags;
    uint16_t len;
    uint8_t *buf;
} i2c_msg_t;

typedef struct
{
    i2c_msg_t *msgs;
    uint16_t num;
} i2c_ioctl_data_t;

typedef struct i2c_driver_s i2c_driver_t;

typedef struct i2c_ops_s
{
    int32_t (*master_xfer)(i2c_driver_t *drv, i2c_msg_t msgs[], uint8_t num);
    int32_t (*slave_xfer)(i2c_driver_t *drv, i2c_msg_t msgs[], uint8_t num);
} i2c_ops_t;

struct i2c_driver_s
{
    driver_t drv;
    i2c_ops_t ops;
};

int32_t i2c_driver_register(const char *name, i2c_driver_t *drv);
i2c_driver_t* i2c_driver_find(const char *name);

int32_t i2c_transfer(i2c_driver_t *drv, i2c_msg_t msgs[], uint16_t num);
int32_t i2c_master_send(i2c_driver_t *drv, uint16_t addr, uint16_t flags, const uint8_t *buf, xsize_t len);
int32_t i2c_master_recv(i2c_driver_t *drv, uint16_t addr, uint16_t flags, uint8_t *buf, xsize_t len);

#ifdef __cplusplus
}
#endif

#endif // __I2C_H__
