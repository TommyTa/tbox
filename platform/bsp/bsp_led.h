/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file led.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef _BSP_LED_H_
#define _BSP_LED_H_

typedef enum {
    BSP_LED_0,
    BSP_LED_1,
} bsp_led_t;

void bsp_led_init(bsp_led_t led);
void bsp_led_on(bsp_led_t led);
void bsp_led_off(bsp_led_t led);
void bsp_led_flip(bsp_led_t led);

#endif
