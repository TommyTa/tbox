/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_pwm.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef _BSP_PWM_H_
#define _BSP_PWM_H_

#define BSP_PWM_FULL    255
#define BSP_PWM_EMPTY   0

#define BSP_PWM_1       0
#define BSP_PWM_2       1
#define BSP_PWM_3       2
#define BSP_PWM_4       3
#define BSP_PWM_5       4
#define BSP_PWM_6       5
#define BSP_PWM_7       6
#define BSP_PWM_8       7
#define BSP_PWM_9       8
#define BSP_PWM_10      9

void bsp_pwm_init(uint8_t pin, uint8_t val);
void bsp_pwm_set(uint8_t pin, uint8_t val);

#endif
