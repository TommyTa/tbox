/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file dev_led.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include <string.h>

#include "typedefs.h"
#include "errorno.h"
#include "modules.h"
#include "log.h"

#include "fuelgauge_dev.h"

static char* fuelgauge_drv_match_ids[] = {
    "gauge-adc",
};

static int32_t fuelgauge_dev_open(device_t *dev)
{
    uint8_t i = 0;
    fuelgauge_driver_t *pdrv_gauge;

    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (dev->dev_drv != NULL)
    {
        return RETVAL(E_OK);
    }

    for (i=0; i<ARRAY_SIZE(fuelgauge_drv_match_ids); i++)
    {
        pdrv_gauge = fuelgauge_driver_find(fuelgauge_drv_match_ids[i]);
        if (pdrv_gauge)
        {
            if (fuelgauge_driver_probe(pdrv_gauge) == RETVAL(E_OK))
            {
                dev->dev_drv = (void*)pdrv_gauge;
                return RETVAL(E_OK);
            }
        }
    }

    return RETVAL(E_FAIL);
}

static int32_t fuelgauge_dev_ioctl(device_t *dev, uint32_t cmd, void *args)
{
    switch (cmd)
    {
        case DEVICE_CTRL_CONFIG:
        {
            fuelgauge_driver_init((fuelgauge_driver_t*)dev->dev_drv);
        }
        break;
        default:
        break;
    }

    return RETVAL(E_OK);
}

const device_ops_t fuelgauge_dev_ops =
{
   /* (*open)  */ fuelgauge_dev_open,
   /* (*close) */ NULL,
   /* (*read)  */ NULL,
   /* (*write) */ NULL,
   /* (*ioctl) */ fuelgauge_dev_ioctl,
};

static void fuelgauge_dev_init(void)
{
    static device_t fuelgauge_dev;
    int32_t ret;

    memset(&fuelgauge_dev, 0, sizeof(device_t));
    fuelgauge_dev.type = DEVICE_CLASS_CHAR;
    fuelgauge_dev.dev_ops = &fuelgauge_dev_ops;

    ret = device_register(&fuelgauge_dev, "dev-gauge", DEVICE_FLAG_WRONLY);
    if (ret < 0)
    {
        LOG_E("[fuelgauge_dev][init]:register failed ret=%d\r\n", ret);
    }
}

DEVICE_INITCALL("dev-gauge", fuelgauge_dev_init);
