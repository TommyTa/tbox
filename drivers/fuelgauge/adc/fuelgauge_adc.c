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
#include "modules.h"

#include "fuelgauge.h"

static int32_t fuelgauge_adc_init(void)
{
    return RETVAL(E_OK);
}

static int32_t fuelgauge_adc_set_model(const void *model)
{
    return RETVAL(E_OK);
}

static int32_t fuelgauge_adc_read_data(fuelgauge_data_t *data)
{
    return RETVAL(E_OK);
}

static int32_t fuelgauge_adc_probe(void)
{
    return RETVAL(E_OK);
}

static int32_t fuelgauge_adc_remove(void)
{
    return RETVAL(E_OK);
}

static int32_t fuelgauge_adc_shutdown(driver_t *drv)
{
    return RETVAL(E_OK);
}

static int32_t fuelgauge_adc_suspend(driver_t *drv)
{
    return RETVAL(E_OK);
}

static int32_t fuelgauge_adc_resume(driver_t *drv)
{
    return RETVAL(E_OK);
}

static driver_pm_ops_t fuelgauge_adc_pm = {
    .shutdown = fuelgauge_adc_shutdown,
    .suspend = fuelgauge_adc_suspend,
    .resume = fuelgauge_adc_resume,
};

static fuelgauge_driver_t fuelgauge_adc = {
    .drv = {
        .probe = fuelgauge_adc_probe,
        .remove = fuelgauge_adc_remove,
        .pm_ops = &fuelgauge_adc_pm,
    },
    .ops = {
        .init = fuelgauge_adc_init,
        .set_model = fuelgauge_adc_set_model,
        .read_data = fuelgauge_adc_read_data,
    }
};

static void fuelgauge_adc_register(void)
{
    int32_t ret;

    ret = fuelgauge_driver_register("gauge-adc", &fuelgauge_adc);
    if (ret != RETVAL(E_OK))
    {
        ;
    }
}

DRIVER_INITCALL("gauge-adc", fuelgauge_adc_register);
