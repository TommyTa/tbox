/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file fuelgauge.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef __FUEL_GAUGE_H__
#define __FUEL_GAUGE_H__

#include "typedefs.h"
#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int16_t voltage;                /* voltage of battery */
    int16_t current;                /* charging current */
    int16_t soc;                    /* state of charge */
    int16_t temperature;            /* temperature of battery */
    int16_t cycle;                  /* cycle of charging */
    int16_t soh;                    /* state of health */
    int16_t fcc;                    /* full charge capacity */
} fuelgauge_data_t;

typedef struct fuelgauge_driver_s fuelgauge_driver_t;

typedef struct fuelgauge_ops_s
{
    int32_t (*init)(void);
    int32_t (*set_model)(const void *model);
    int32_t (*read_data)(fuelgauge_data_t *data);
} fuelgauge_ops_t;

struct fuelgauge_driver_s
{
    driver_t drv;
    fuelgauge_ops_t ops;
};

int32_t fuelgauge_driver_register(const char *name, fuelgauge_driver_t *drv);
fuelgauge_driver_t* fuelgauge_driver_find(const char *name);

int32_t fuelgauge_driver_probe(fuelgauge_driver_t *drv);
int32_t fuelgauge_driver_init(fuelgauge_driver_t *drv);
int32_t fuelgauge_driver_set_model(fuelgauge_driver_t *drv, const void *model);
int32_t fuelgauge_driver_read_data(fuelgauge_driver_t *drv, fuelgauge_data_t *data);

#ifdef __cplusplus
}
#endif

#endif // __FUEL_GAUGE_H__
