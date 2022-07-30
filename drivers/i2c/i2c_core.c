/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file i2c_core.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include "log.h"
#include "errorno.h"

#include "i2c.h"

int32_t i2c_driver_register(const char *name, i2c_driver_t *drv)
{
    int32_t ret;
    driver_t *pdrv;

    pdrv = &(drv->drv);

    pdrv->drv_data = (void*)drv;
    pdrv->type = DRIVER_CLASS_I2C;

    /* register to driver manager */
    ret = driver_register(pdrv, name);

    return ret;
}

i2c_driver_t* i2c_driver_find(const char *name)
{
    i2c_driver_t *i2c_drv;
    driver_t *pdrv;

    pdrv = driver_find(name);
    if (pdrv == NULL || pdrv->type != DRIVER_CLASS_I2C)
    {
        return NULL;
    }

    i2c_drv = (i2c_driver_t*)pdrv->drv_data;

    return i2c_drv;
}


int32_t i2c_transfer(i2c_driver_t *drv, i2c_msg_t msgs[], uint16_t num)
{
    int32_t ret;

    if (drv->ops.master_xfer)
    {
        // rt_mutex_take(&bus->lock, RT_WAITING_FOREVER);
        ret = drv->ops.master_xfer(drv, msgs, num);
        // rt_mutex_release(&bus->lock);

        return ret;
    }

    return RETVAL(E_NOT_SUPPORT);
}

int32_t i2c_master_send(i2c_driver_t *drv, uint16_t addr, uint16_t flags,
                        const uint8_t *buf, xsize_t len)
{
    int32_t ret;
    i2c_msg_t msg;

    msg.addr  = addr;
    msg.flags = flags;
    msg.len   = len;
    msg.buf   = (uint8_t *)buf;

    ret = i2c_transfer(drv, &msg, 1);

    return (ret > 0) ? len : ret;
}

int32_t i2c_master_recv(i2c_driver_t *drv, uint16_t addr, uint16_t flags,
                        uint8_t *buf, xsize_t len)
{
    int32_t ret;
    i2c_msg_t msg;

    msg.addr   = addr;
    msg.flags  = flags | I2C_DEV_FLAG_RD;
    msg.len    = len;
    msg.buf    = buf;

    ret = i2c_transfer(drv, &msg, 1);

    return (ret > 0) ? len : ret;
}
