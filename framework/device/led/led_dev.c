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
#include "log.h"
#include "modules.h"
#include "led.h"

#include "led_dev.h"

static char* led_drv_match_ids[] = {
    "led-aw20x",
};

static int32_t led_dev_open(device_t *dev)
{
    uint8_t i = 0;
    led_driver_t *pdrv_led;

    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (dev->dev_drv != NULL)
    {
        return RETVAL(E_OK);
    }

    for (i = 0; i < ARRAY_SIZE(led_drv_match_ids); i++)
    {
        pdrv_led = led_driver_find(led_drv_match_ids[i]);
        if (pdrv_led)
        {
            if (led_driver_probe(pdrv_led) == RETVAL(E_OK))
            {
                dev->dev_drv = (void*)pdrv_led;
                led_driver_init(pdrv_led);
                return RETVAL(E_OK);
            }
        }
    }

    return RETVAL(E_FAIL);
}

static int32_t led_dev_ioctl(device_t *dev, uint32_t cmd, void *args)
{
    led_driver_t *pdrv_led = (led_driver_t*)dev->dev_drv;

    switch (cmd)
    {
        case DEVICE_CTRL_CONFIG:
        {
            led_ind_t* led_ctrl = (led_ind_t*)args;
            led_driver_set_pattern(pdrv_led, led_ctrl->chan, led_ctrl->pattern);
        }
        break;
        case DEVICE_CTRL_STATE:
        {
            led_dev_state_t* led_dev_state = (led_dev_state_t*)args;
            led_driver_get_state(pdrv_led, led_dev_state);
        }
        break;
        default:
        break;
    }

    return RETVAL(E_OK);
}

const device_ops_t led_dev_ops =
{
   /* (*open)  */ led_dev_open,
   /* (*close) */ NULL,
   /* (*read)  */ NULL,
   /* (*write) */ NULL,
   /* (*ioctl) */ led_dev_ioctl,
};

static void led_dev_init(void)
{
    static device_t led_dev;
    int32_t ret;

    memset(&led_dev, 0, sizeof(led_dev));
    led_dev.type = DEVICE_CLASS_CHAR;
    led_dev.dev_ops = &led_dev_ops;

    ret = device_register(&led_dev, "dev-led", DEVICE_FLAG_WRONLY);
    if (ret < RETVAL(E_OK))
    {
        LOG_E("[dev_led][dev_led_init]:register failed ret=%d\r\n", ret);
    }
}

DEVICE_INITCALL("dev-led", led_dev_init);
