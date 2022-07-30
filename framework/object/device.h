/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file device.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "typedefs.h"
#include "object.h"
#include "driver.h"

/**
 * device flags defitions
 */
#define DEVICE_FLAG_DEACTIVATE          0x0000  /**< device is not not initialized */

#define DEVICE_FLAG_RDONLY              0x0001  /**< read only */
#define DEVICE_FLAG_WRONLY              0x0002  /**< write only */
#define DEVICE_FLAG_RDWR                0x0003  /**< read and write */

#define DEVICE_FLAG_REMOVABLE           0x0004  /**< removable device */
#define DEVICE_FLAG_STANDALONE          0x0008  /**< standalone device */
#define DEVICE_FLAG_ACTIVATED           0x0010  /**< device is activated */
#define DEVICE_FLAG_SUSPENDED           0x0020  /**< device is suspended */
#define DEVICE_FLAG_STREAM              0x0040  /**< stream mode */

#define DEVICE_FLAG_INT_RX              0x0100  /**< INT mode on Rx */
#define DEVICE_FLAG_DMA_RX              0x0200  /**< DMA mode on Rx */
#define DEVICE_FLAG_INT_TX              0x0400  /**< INT mode on Tx */
#define DEVICE_FLAG_DMA_TX              0x0800  /**< DMA mode on Tx */

#define DEVICE_OFLAG_CLOSE              0x0000  /**< device is closed */
#define DEVICE_OFLAG_RDONLY             0x0001  /**< read only access */
#define DEVICE_OFLAG_WRONLY             0x0002  /**< write only access */
#define DEVICE_OFLAG_RDWR               0x0003  /**< read and write */
#define DEVICE_OFLAG_OPEN               0x0008  /**< device is opened */
#define DEVICE_OFLAG_MASK               0x0F0F  /**< mask of open flag */

/**
 * general device commands
 */
#define DEVICE_CTRL_RESUME              0x0100  /**< resume device */
#define DEVICE_CTRL_SUSPEND             0x0200  /**< suspend device */
#define DEVICE_CTRL_CONFIG              0x0300  /**< configure device */
#define DEVICE_CTRL_STATE               0x0400  /**< get device state */

#define DEVICE_CTRL_SET_INT             0x1000  /**< set interrupt */
#define DEVICE_CTRL_CLR_INT             0x1100  /**< clear interrupt */
#define DEVICE_CTRL_GET_INT             0x1200  /**< get interrupt status */

/**
 * special device commands
 */
#define DEVICE_CTRL_CHAR_STREAM         0x3000  /**< stream mode on char device */

#define DEVICE_CTRL_BLK_GETGEOME        0x4000  /**< get geometry information   */
#define DEVICE_CTRL_BLK_SYNC            0x4100  /**< flush data to block device */
#define DEVICE_CTRL_BLK_ERASE           0x4200  /**< erase block on block device */
#define DEVICE_CTRL_BLK_AUTOREFRESH     0x4300  /**< block device : enter/exit auto refresh mode */

#define DEVICE_CTRL_NETIF_GETMAC        0x5000  /**< get mac address */

#define DEVICE_CTRL_MTD_FORMAT          0x6000  /**< format a MTD device */

#define DEVICE_CTRL_RTC_GET_TIME        0x7000  /**< get time */
#define DEVICE_CTRL_RTC_SET_TIME        0x7100  /**< set time */
#define DEVICE_CTRL_RTC_GET_ALARM       0x7200  /**< get alarm */
#define DEVICE_CTRL_RTC_SET_ALARM       0x7300  /**< set alarm */

/**
 * device (I/O) class type
 */
typedef enum
{
    DEVICE_CLASS_CHAR,                          /**< character device */
    DEVICE_CLASS_BLOCK,                         /**< block device */
    DEVICE_CLASS_NETIF,                         /**< net interface */
    DEVICE_CLASS_MTD,                           /**< memory device */
    DEVICE_CLASS_CAN,                           /**< CAN device */
    DEVICE_CLASS_RTC,                           /**< RTC device */
    DEVICE_CLASS_SOUND,                         /**< Sound device */
    DEVICE_CLASS_GRAPHIC,                       /**< Graphic device */
    DEVICE_CLASS_I2C_BUS,                       /**< I2C bus device */
    DEVICE_CLASS_USB_DEVICE,                    /**< USB slave device */
    DEVICE_CLASS_USB_HOST,                      /**< USB host bus */
    DEVICE_CLASS_SPI_BUS,                       /**< SPI bus device */
    DEVICE_CLASS_SPI_DEVICE,                    /**< SPI device */
    DEVICE_CLASS_SDIO,                          /**< SDIO bus device */
    DEVICE_CLASS_PM,                            /**< PM pseudo device */
    DEVICE_CLASS_PIPE,                          /**< Pipe device */
    DEVICE_CLASS_PORTAL,                        /**< Portal device */
    DEVICE_CLASS_TIMER,                         /**< Timer device */
    DEVICE_CLASS_MISCELLANEOUS,                 /**< Miscellaneous device */
    DEVICE_CLASS_SENSOR,                        /**< Sensor device */
    DEVICE_CLASS_TOUCH,                         /**< Touch device */
    DEVICE_CLASS_UNKNOWN                        /**< unknown device */
} device_class_t;

typedef struct device_s device_t;
typedef struct device_ops_s device_ops_t;

/**
 * operations set for device object
 */
struct device_ops_s
{
    int32_t (*open)(device_t *dev);
    int32_t (*close)(device_t *dev);
    xssize_t (*read)(device_t *dev, char *buffer, xsize_t size, xloff_t *pos);
    xssize_t (*write)(device_t *dev, const char *buffer, xsize_t size, xloff_t *pos);
    int32_t (*ioctl)(device_t *dev, uint32_t cmd, void *args);
};

/**
 * Device structure
 */
struct device_s
{
    object_t            obj;                    /**< inherit from rt_object */

    device_class_t      type;                   /**< device type */
    xflag_t             flag;                   /**< device flag */
    xflag_t             oflag;                  /**< device open flag */

    uint8_t             ref_cnt;                /**< reference count */
    uint8_t             dev_id;                 /**< 0 - 255 */

    const device_ops_t  *dev_ops;               /**< common device interface */
    driver_t            *dev_drv;               /**< device driver */
    void                *dev_data;              /**< device private data */
};

int32_t device_register(device_t *dev, const char *name, xflag_t flags);
int32_t device_unregister(device_t *dev);

device_t* device_find(const char *name);
int32_t device_open(device_t *dev, xflag_t oflag);
int32_t device_close(device_t *dev);
xssize_t device_read(device_t *dev, char *buffer, xsize_t size, xloff_t *pos);
xssize_t device_write(device_t *dev, const char *buffer, xsize_t size, xloff_t *pos);
int32_t device_ioctl(device_t *dev, int cmd, void *arg);

#endif // _DEVICE_H_
