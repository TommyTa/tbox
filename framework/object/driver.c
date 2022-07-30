/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file driver.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include "errorno.h"

#include "driver.h"

#define DRIVER_ENTER_CRITICAL()
#define DRIVER_EXIT_CRITICAL()

/**
 * This function finds a driver by specified name.
 *
 * @param name the driver's name
 *
 * @return the registered driver on successful, or NULL on failure.
 */
driver_t* driver_find(const char *name)
{
    driver_t *obj;
    obj = (driver_t*)object_find(OBJECT_CLASS_DRIVER, name);
    return obj;
}

/**
 * This function registers a driver with specified name.
 *
 * @param drv the pointer of driver structure
 * @param name the driver's name
 *
 * @return the error code, EOK on initialization successfully.
 */
int32_t driver_register(driver_t *drv, const char *name)
{
    int32_t ret;

    if (drv == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (driver_find(name) != NULL)
    {
        return RETVAL(E_FAIL);
    }

    ret = object_attack(&(drv->obj), OBJECT_CLASS_DRIVER, name);

    return ret;
}

/**
 * This function removes a previously registered driver
 *
 * @param drv the pointer of driver structure
 *
 * @return the error code, EOK on successfully.
 */
int32_t driver_unregister(driver_t *drv)
{
    int32_t ret;

    if (drv == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (object_get_type(&(drv->obj)) != OBJECT_CLASS_DRIVER)
    {
        return RETVAL(E_NO_DEV);
    }

    ret = object_detach(&(drv->obj));

    return ret;
}

/**
 * This function will probe the specified driver
 *
 * @param drv the pointer of driver structure
 *
 * @return the result
 */
int32_t driver_probe(driver_t *drv)
{
    int32_t ret = RETVAL(E_OK);

    if (drv == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (drv->probe == NULL)
    {
        return RETVAL(E_NOT_SUPPORT);
    }

    ret = drv->probe();

    return ret;
}

/**
 * This function will remove the specified driver
 *
 * @param drv the pointer of driver structure
 *
 * @return the result
 */
int32_t driver_remove(driver_t *drv)
{
    int32_t ret = RETVAL(E_OK);

    if (drv == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (drv->remove == NULL)
    {
        return RETVAL(E_NOT_SUPPORT);
    }

    ret = drv->remove();

    return ret;
}

/**
 * This function will set the reception indication callback function. This callback function
 * is invoked when this driver receives data.
 *
 * @param drv the pointer of driver structure
 * @param rx_ind the indication callback function
 *
 * @return EOK
 */
void driver_set_rx_ind_cbk(driver_t *drv, drv_rx_ind_cbk_t rx_ind)
{
    if (drv != NULL)
    {
        drv->rx_ind_cbk = rx_ind;
    }
}

/**
 * This function will set the indication callback function when device has
 * written data to physical hardware.
 *
 * @param drv the pointer of driver structure
 * @param tx_done the indication callback function
 *
 * @return EOK
 */
void driver_set_tx_done_cbk(driver_t *drv, drv_tx_done_cbk_t tx_done)
{
    if (drv != NULL)
    {
        drv->tx_done_cbk = tx_done;
    }
}

/**
 * This function will sent rx indicate
 *
 * @param drv the pointer of driver structure
 *
 * @return the result
 */
void driver_rx_ind_to_upper(driver_t *drv, void *buffer, uint16_t size)
{
    if (drv != NULL && drv->rx_ind_cbk != NULL)
    {
        drv->rx_ind_cbk(buffer, size);
    }
}

/**
 * This function will sent the tx done message
 *
 * @param drv the pointer of driver structure
 *
 * @return the result
 */
void driver_tx_done_to_upper(driver_t *drv, void *buffer, uint16_t size)
{
    if (drv != NULL && drv->tx_done_cbk != NULL)
    {
        drv->tx_done_cbk(buffer, size);
    }
}

/**
 * This function will set the private data
 *
 * @param drv the pointer of driver structure
 *
 * @return the result
 */
void driver_set_drvdata(driver_t *drv, void* drv_data)
{
    if (drv != NULL)
    {
        drv->drv_data = drv_data;
    }
}

/**
 * This function will get the private data
 *
 * @param drv the pointer of driver structure
 *
 * @return the result
 */
void* driver_get_drvdata(driver_t *drv)
{
    if (drv == NULL)
    {
        return NULL;
    }

    return drv->drv_data;
}

