/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_gpio.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include "typedefs.h"

typedef enum
{
    BSP_GPIO_0                   = 0,
    BSP_GPIO_1,
    BSP_GPIO_2,
    BSP_GPIO_NUM,
} bsp_gpio_t;

typedef enum
{
    BSP_GPIO_EXTI_DISABLE       = 0,
    BSP_GPIO_EXTI_ENABLE
} bsp_gpio_exti_cmd_t;

typedef void (*bsp_gpio_cb_t)(void);

void bsp_gpio_init(bsp_gpio_t gpio, bsp_gpio_exti_cmd_t cmd);
int32_t bsp_gpio_register_irq_callback(bsp_gpio_t gpio, bsp_gpio_cb_t cb);
void bsp_gpio_irq_handler(bsp_gpio_t gpio);
uint8_t bsp_gpio_read(bsp_gpio_t gpio);

#endif // __BSP_GPIO_H__
