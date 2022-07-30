/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file fuelgauge.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include "log.h"
#include "errorno.h"

#include "fuelgauge.h"

int32_t fuelgauge_driver_register(const char *name, fuelgauge_driver_t *drv)
{
    int32_t ret;
    driver_t *pdrv;

    pdrv = &(drv->drv);

    pdrv->drv_data = (void*)drv;
    pdrv->type = DRIVER_CLASS_FULE_GAUGE;

    /* register to driver manager */
    ret = driver_register(pdrv, name);

    return ret;
}

fuelgauge_driver_t* fuelgauge_driver_find(const char *name)
{
    fuelgauge_driver_t *fuelgauge;
    driver_t *pdrv;

    pdrv = driver_find(name);
    if (pdrv == NULL || pdrv->type != DRIVER_CLASS_FULE_GAUGE)
    {
        return NULL;
    }

    fuelgauge = (fuelgauge_driver_t*)pdrv->drv_data;

    return fuelgauge;
}

int32_t fuelgauge_driver_probe(fuelgauge_driver_t *drv)
{
    int32_t ret;

    if (drv == NULL)
    {
        return RETVAL(E_NO_DEV);
    }

    ret = driver_probe(&drv->drv);

    return ret;
}

int32_t fuelgauge_driver_init(fuelgauge_driver_t *drv)
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

int32_t fuelgauge_driver_set_model(fuelgauge_driver_t *drv, const void *model)
{
    if (drv == NULL)
    {
        return RETVAL(E_NO_DEV);
    }

    if (drv->ops.set_model)
    {
        return drv->ops.set_model(model);
    }

    return RETVAL(E_NOT_SUPPORT);
}

int32_t fuelgauge_driver_read_data(fuelgauge_driver_t *drv, fuelgauge_data_t *data)
{
    if (drv == NULL)
    {
        return RETVAL(E_NO_DEV);
    }

    if (drv->ops.read_data)
    {
        return drv->ops.read_data(data);
    }

    return RETVAL(E_NOT_SUPPORT);
}
