/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file main.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdio.h>
#include <stdint.h>

#include "sys_cmsis.h"
#include "modules.h"
#include "bsp_uart.h"
#include "bsp_led.h"
#include "bsp_i2c.h"
#include "bsp_spi.h"
#include "log.h"
#include "ui_indicator.h"

void SysTick_Handler(void)
{
    sys_tick_handler();
}

/**
 * @brief
 * @param
 * @retval
 */
void hardware_init(void)
{
    sys_set_nvic();
    bsp_uart_init(BSP_UART0);
    bsp_uart_init(BSP_UART1);
    bsp_uart_init(BSP_UART2);
    bsp_i2c_init(BSP_I2C_BUS1);
    bsp_i2c_init(BSP_I2C_BUS2);
}

/**
 * @brief
 * @param
 * @retval
 */
void start_kernel(void)
{
    sys_kernel_start();
}

/**
 * @brief  Main function.
 * @param
 * @retval
 */
int main(void)
{
    hardware_init();
    modules_setup();

    LOG_W("[main]:application start...\r\n");

    start_kernel();
    while (1)
    {
        ;
    }
}
