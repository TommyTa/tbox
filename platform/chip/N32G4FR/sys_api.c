/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file sys_api.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include "n32g4fr.h"

void sys_reset(void)
{
    NVIC_SystemReset();
}

void sys_irq_disable(void)
{
    __disable_irq();
}

void sys_irq_enable(void)
{
    __enable_irq();
}

void sys_set_nvic(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}
