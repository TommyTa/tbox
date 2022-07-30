/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file led_core.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include "log.h"
#include "errorno.h"

#include "led.h"

int32_t led_driver_register(const char *name, led_driver_t *drv)
{
    int32_t ret;
    driver_t *pdrv;

    pdrv = &(drv->drv);

    pdrv->drv_data = (void*)drv;
    pdrv->type = DRIVER_CLASS_LED;

    /* register to driver manager */
    ret = driver_register(pdrv, name);

    return ret;
}

led_driver_t* led_driver_find(const char *name)
{
    led_driver_t *pled;
    driver_t *pdrv;

    pdrv = driver_find(name);
    if (pdrv == NULL || pdrv->type != DRIVER_CLASS_LED)
    {
        return NULL;
    }

    pled = (led_driver_t*)pdrv->drv_data;

    return pled;
}

int32_t led_driver_probe(led_driver_t *drv)
{
    int32_t ret;

    if (drv == NULL)
    {
        return RETVAL(E_NO_DEV);
    }

    ret = driver_probe(&drv->drv);

    return ret;
}

int32_t led_driver_init(led_driver_t *drv)
{
    if (drv == NULL)
    {
        return RETVAL(E_NO_DEV);
    }

    if (drv->ops.init)
    {
        return drv->ops.init();
    }

    return RETVAL(E_NOT_SUPPORT);
}

int32_t led_driver_set_pattern(led_driver_t *drv, led_chan_t chan, const led_pattern_t *pattern)
{
    if (drv == NULL)
    {
        return RETVAL(E_NO_DEV);
    }

    if (drv->ops.set_pattern)
    {
        return drv->ops.set_pattern(chan, pattern);
    }

    return RETVAL(E_NOT_SUPPORT);
}

int32_t led_driver_set_state(led_driver_t *drv, led_chan_t chan, led_state_t state)
{
    if (drv == NULL)
    {
        return RETVAL(E_NO_DEV);
    }

    if (drv->ops.set_state)
    {
        return drv->ops.set_state(chan, state);
    }

    return RETVAL(E_NOT_SUPPORT);
}

int32_t led_driver_get_state(led_driver_t *drv, led_dev_state_t *led_dev)
{
    if (drv == NULL)
    {
        return RETVAL(E_NO_DEV);
    }

    if (drv->ops.get_state)
    {
        return drv->ops.get_state(led_dev);
    }

    return RETVAL(E_NOT_SUPPORT);
}
