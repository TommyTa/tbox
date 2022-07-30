/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file dev_led.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _LED_DEV_H_
#define _LED_DEV_H_

#include "../../object/device.h"
#include "led.h"

#define CMD_LED_STATE             0x00

typedef struct {
    uint8_t chan;
    const led_pattern_t *pattern;
} led_ind_t;

#endif // _LED_DEV_H_
