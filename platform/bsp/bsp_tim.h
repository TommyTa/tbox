/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_tim.h
 * @author 
 * @version v1.0.0
 * **************************************************************************/

#ifndef _BSP_TIM_H_
#define _BSP_TIM_H_

#include "n32g4fr.h"

typedef void (*bsp_tim3_cb_t)(void);

typedef struct
{
    uint8_t num;      //定时器编号 范围1-8
    TIM_Module *timx; //定时器寄存器结构体,如TIM3
    uint32_t rcc;     //定时器时钟,如RCC_APB1_PERIPH_TIM3
    uint8_t irqn;     //中断编号,如TIM3_IRQn
} bsp_tim_cfg;

extern bsp_tim_cfg bsp_tim3_cfg;

int32_t bsp_tim_init(bsp_tim_cfg timx, uint16_t Period, uint16_t Prescaler);
void bsp_tim_enable(bsp_tim_cfg timx, FunctionalState Cmd);
int32_t bsp_tim3_set_irq_callback(bsp_tim3_cb_t cb);

#endif
