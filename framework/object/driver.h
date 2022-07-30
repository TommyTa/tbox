/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file driver.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "typedefs.h"
#include "object.h"

/**
 * driver class type
 */
typedef enum
{
    DRIVER_CLASS_NET,                                   /** net driver */
    DRIVER_CLASS_MTD,                                   /** memory driver */
    DRIVER_CLASS_CAN,                                   /** CAN driver */
    DRIVER_CLASS_RTC,                                   /** RTC driver */
    DRIVER_CLASS_SOUND,                                 /** Sound driver */
    DRIVER_CLASS_LED,                                   /** LED driver */
    DRIVER_CLASS_GRAPHIC,                               /** Graphic driver */
    DRIVER_CLASS_I2C,                                   /** I2C bus driver */
    DRIVER_CLASS_USB_DEVICE,                            /** USB slave driver */
    DRIVER_CLASS_USB_HOST,                              /** USB host bus */
    DRIVER_CLASS_SPI_BUS,                               /** SPI bus driver */
    DRIVER_CLASS_SPI_DEVICE,                            /** SPI driver */
    DRIVER_CLASS_SDIO,                                  /** SDIO bus driver */
    DRIVER_CLASS_SENSOR,                                /** Sensor driver */
    DRIVER_CLASS_TOUCH,                                 /** Touch driver */
    DRIVER_CLASS_FULE_GAUGE,                            /** Fuel gauge driver */
    DRIVER_CLASS_CHARGER,                               /** Charger driver */
    DRIVER_CLASS_UNKNOWN                                /** unknown driver */
} driver_class_t;

typedef struct driver_s driver_t;

/**
 * Operations set for driver object
 */
typedef struct
{
    int32_t (*shutdown)(driver_t *drv);
    int32_t (*suspend)(driver_t *drv);
    int32_t (*resume)(driver_t *dev);
} driver_pm_ops_t;

typedef void (*drv_rx_ind_cbk_t)(void *buffer, uint16_t size);
typedef void (*drv_tx_done_cbk_t)(void *buffer, uint16_t size);

/**
 * Driver structure
 */
struct driver_s
{
    object_t                obj;                        /** inherit from object */
    driver_class_t          type;                       /** driver type */
    int32_t                 (*probe)(void);             /** driver probe */
    int32_t                 (*remove)(void);            /** driver remove */
    drv_rx_ind_cbk_t        rx_ind_cbk;                 /** RX callback */
    drv_tx_done_cbk_t       tx_done_cbk;                /** TX callback */
    const driver_pm_ops_t   *pm_ops;                    /** common interface */
    void                    *drv_data;                  /** driver private data */
};

int32_t driver_register(driver_t *drv, const char *name);
int32_t driver_unregister(driver_t *drv);

driver_t* driver_find(const char *name);
int32_t driver_probe(driver_t *drv);
int32_t driver_remove(driver_t *drv);

void driver_set_rx_ind_cbk(driver_t *drv, drv_rx_ind_cbk_t rx_ind);
void driver_set_tx_done_cbk(driver_t *drv, drv_tx_done_cbk_t tx_done);

void driver_rx_ind_to_upper(driver_t *drv, void *buffer, uint16_t size);
void driver_tx_done_to_upper(driver_t *drv, void *buffer, uint16_t size);

void driver_set_drvdata(driver_t *drv, void* drv_data);
void* driver_get_drvdata(driver_t *drv);

#endif // _DRIVER_H_
