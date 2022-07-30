/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file main.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdio.h>

#include "typedefs.h"
#include "errorno.h"
#include "bsp_flash.h"

#include "dfu.h"

void SysTick_Handler(void)
{
}

int main(void)
{
    int32_t ret = RETVAL(E_OK);
    uint16_t flag = 0;

    flag = (bsp_flash_read_nv() & 0x00ff);
    if (flag == ACTION_CARRY_TO_APP)
    {
        ret = dfu_carry_app();
    }
    else if(flag == ACTION_UPGRADE_APP)
    {
        goto dfu;
    }

    if (ret == RETVAL(E_OK))
    {
        dfu_jump_app();
    }

dfu:
    enter_dfu_mode();
    for (;;)
    {
        
    }
}
