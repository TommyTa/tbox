/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file device.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include "errorno.h"

#include "device.h"

#define DEVICE_ENTER_CRITICAL()
#define DEVICE_EXIT_CRITICAL()

#define device_ops_open             (dev->dev_ops->open)
#define device_ops_close            (dev->dev_ops->close)
#define device_ops_read             (dev->dev_ops->read)
#define device_ops_write            (dev->dev_ops->write)
#define device_ops_ioctl            (dev->dev_ops->ioctl)

/**
 * This function finds a device driver by specified name.
 *
 * @param name the device driver's name
 *
 * @return the registered device driver on successful, or NULL on failure.
 */
device_t* device_find(const char *name)
{
    device_t *obj;
    obj = (device_t*)object_find(OBJECT_CLASS_DEVICE, name);
    return obj;
}

/**
 * This function registers a device driver with specified name.
 *
 * @param dev the pointer of device driver structure
 * @param name the device driver's name
 * @param flags the capabilities flag of device
 *
 * @return the error code, EOK on initialization successfully.
 */
int32_t device_register(device_t *dev, const char *name, xflag_t flags)
{
    int32_t ret;

    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (device_find(name) != NULL)
    {
        return RETVAL(E_FAIL);
    }

    dev->flag = flags;
    dev->oflag = DEVICE_OFLAG_CLOSE;
    dev->ref_cnt = 0;

    ret = object_attack(&(dev->obj), OBJECT_CLASS_DEVICE, name);

    return ret;
}

/**
 * This function removes a previously registered device driver
 *
 * @param dev the pointer of device driver structure
 *
 * @return the error code, EOK on successfully.
 */
int32_t device_unregister(device_t *dev)
{
    int32_t ret;

    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (object_get_type(&(dev->obj)) != OBJECT_CLASS_DEVICE)
    {
        return RETVAL(E_NO_DEV);
    }

    ret = object_detach(&(dev->obj));

    return ret;
}

/**
 * This function will open a device
 *
 * @param dev the pointer of device driver structure
 * @param oflag the flags for device open
 *
 * @return the result
 */
int32_t device_open(device_t *dev, xflag_t oflag)
{
    int32_t ret = RETVAL(E_OK);

    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (object_get_type(&(dev->obj)) != OBJECT_CLASS_DEVICE)
    {
        return RETVAL(E_NO_DEV);
    }

    /* device is a stand alone device and opened */
    if ((dev->flag & DEVICE_FLAG_STANDALONE) &&
        (dev->oflag & DEVICE_OFLAG_OPEN))
    {
        return RETVAL(E_BUSY);
    }

    /* call device_open interface */
    if (device_ops_open != NULL)
    {
        ret = device_ops_open(dev);
    }
    else
    {
        /* set open flag */
        dev->oflag = (oflag & DEVICE_OFLAG_MASK);
    }

    /* set open flag */
    if (ret == RETVAL(E_OK))
    {
        dev->oflag |= DEVICE_OFLAG_OPEN;
        dev->ref_cnt ++;
    }

    return ret;
}

/**
 * This function will close a device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
int32_t device_close(device_t *dev)
{
    int32_t ret = RETVAL(E_OK);

    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (object_get_type(&(dev->obj)) != OBJECT_CLASS_DEVICE)
    {
        return RETVAL(E_NO_DEV);
    }

    if (dev->ref_cnt == 0)
    {
        return RETVAL(E_STATE);
    }

    dev->ref_cnt --;
    if (dev->ref_cnt != 0)
    {
        return RETVAL(E_OK);
    }

    /* call device_close interface */
    if (device_ops_close != NULL)
    {
        ret = device_ops_close(dev);
    }

    /* set open flag */
    if (ret == RETVAL(E_OK))
    {
        dev->oflag = DEVICE_OFLAG_CLOSE;
    }

    return ret;
}

/**
 * This function will read some data from a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of reading
 * @param buffer the data buffer to save read data
 * @param size the size of buffer
 *
 * @return the actually read size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
xssize_t device_read(device_t *dev, char *buffer, xsize_t size, xloff_t *pos)
{
    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (object_get_type(&(dev->obj)) != OBJECT_CLASS_DEVICE)
    {
        return RETVAL(E_NO_DEV);
    }

    if (dev->ref_cnt == 0)
    {
        return RETVAL(E_STATE);
    }

    /* call device_read interface */
    if (device_ops_read != NULL)
    {
        return device_ops_read(dev, buffer, size, pos);
    }

    return RETVAL(E_NOT_SUPPORT);
}

/**
 * This function will write some data to a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of written
 * @param buffer the data buffer to be written to device
 * @param size the size of buffer
 *
 * @return the actually written size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
xssize_t device_write(device_t *dev, const char *buffer, xsize_t size, xloff_t *pos)
{
    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (object_get_type(&(dev->obj)) != OBJECT_CLASS_DEVICE)
    {
        return RETVAL(E_NO_DEV);
    }

    if (dev->ref_cnt == 0)
    {
        return RETVAL(E_STATE);
    }

    /* call device_write interface */
    if (device_ops_write != NULL)
    {
        return device_ops_write(dev, buffer, size, pos);
    }

    return RETVAL(E_NOT_SUPPORT);
}

/**
 * This function will perform a variety of control functions on devices.
 *
 * @param dev the pointer of device driver structure
 * @param cmd the command sent to device
 * @param arg the argument of command
 *
 * @return the result
 */
int32_t device_ioctl(device_t *dev, int cmd, void *arg)
{
    if (dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (object_get_type(&(dev->obj)) != OBJECT_CLASS_DEVICE)
    {
        return RETVAL(E_NO_DEV);
    }

    if (dev->ref_cnt == 0)
    {
        return RETVAL(E_STATE);
    }

    /* call device_write interface */
    if (device_ops_ioctl != NULL)
    {
        return device_ops_ioctl(dev, cmd, arg);
    }

    return RETVAL(E_NOT_SUPPORT);
}
