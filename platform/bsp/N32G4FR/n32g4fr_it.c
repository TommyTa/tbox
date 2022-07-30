/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g4fr_it.c
 * @author Nations Solution Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include "n32g4fr_it.h"
#include "n32g4fr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "log.h"

/** @addtogroup N32G4FR_StdPeriph_Template
 * @{
 */

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 */
void NMI_Handler(void)
{
}

void HardFault_Handler_S(unsigned int * hardfault_args)
{
    static unsigned int stacked_r0;
    static unsigned int stacked_r1;
    static unsigned int stacked_r2;
    static unsigned int stacked_r3;
    static unsigned int stacked_r12;
    static unsigned int stacked_lr;
    static unsigned int stacked_pc;
    static unsigned int stacked_psr;
    static unsigned int SHCSR;
    static unsigned char MFSR;
    static unsigned char BFSR;
    static unsigned short int UFSR;
    static unsigned int HFSR;
    static unsigned int DFSR;
    static unsigned int MMAR;
    static unsigned int BFAR;
    static unsigned int AFSR;
    stacked_r0 = ((unsigned long)hardfault_args[0]);
    stacked_r1 = ((unsigned long)hardfault_args[1]);
    stacked_r2 = ((unsigned long)hardfault_args[2]);
    stacked_r3 = ((unsigned long)hardfault_args[3]);
    stacked_r12 = ((unsigned long)hardfault_args[4]);
    stacked_lr = ((unsigned long)hardfault_args[5]);
    stacked_pc = ((unsigned long)hardfault_args[6]);
    stacked_psr = ((unsigned long)hardfault_args[7]);
    SHCSR = (*((volatile unsigned long*)(0xE000ED24)));
    MFSR = (*((volatile unsigned char *)(0xE000ED28)));
    BFSR = (*((volatile unsigned char *)(0xE000ED29)));
    UFSR = (*((volatile unsigned short int *)(0xE000ED2A)));
    HFSR = (*((volatile unsigned long *)(0xE000ED2C)));
    DFSR = (*((volatile unsigned long *)(0xE000ED30)));
    MMAR = (*((volatile unsigned long *)(0xE000ED34)));
    BFAR = (*((volatile unsigned long *)(0xE000ED38)));
    AFSR = (*((volatile unsigned long*)(0xE000ED3C)));
    LOG_E("R0 = 0x%x\n", stacked_r0);
    LOG_E("R1 = 0x%x\n", stacked_r1);
    LOG_E("R2 = 0x%x\n", stacked_r2);
    LOG_E("R3 = 0x%x\n", stacked_r3);
    LOG_E("R12 = 0x%x\n", stacked_r12);
    LOG_E("LR[R14] = 0x%x\n", stacked_lr);
    LOG_E("PC[R15] = 0x%x\n", stacked_pc);
    LOG_E("PSR = 0x%x\n", stacked_psr);
    LOG_E("SCB_SHCSR = 0x%x\n", SCB->SHCSR);
    LOG_E("SHCSR = 0x%x\n", SHCSR);
    LOG_E("MFSR = 0x%x\n", MFSR);
    LOG_E("BFSR = 0x%x\n", BFSR);
    LOG_E("UFSR = 0x%x\n", UFSR);
    LOG_E("HFSR = 0x%x\n", HFSR);
    LOG_E("DFSR = 0x%x\n", DFSR);
    LOG_E("MMAR = 0x%x\n", MMAR);
    LOG_E("BFAR = 0x%x\n", BFAR);
    LOG_E("AFSR = 0x%x\n", AFSR);
    LOG_E("HardFault_Handler\r\n");
    while (1);
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    __asm volatile
    (
        "    tst lr, #4               \n" /* The FPU enable bits are in the CPACR. */
        "    ite eq                   \n"
        "                             \n"
        "    mrseq r0, msp            \n" /* Enable CP10 and CP11 coprocessors, then save back. */
        "    mrsne r0, psp            \n"
        "    b HardFault_Handler_S      "
    );
    LOG_E("HardFault_Handler\r\n");
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Debug Monitor exception.
 */
void DebugMon_Handler(void)
{
}


/******************************************************************************/
/*                 N32G4FR Peripherals Interrupt Handlers                     */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_n32g4fr.s).                                                 */
/******************************************************************************/

/**
 * @brief
 * @param
 * @retval
 */
void UART6_IRQHandler(void)
{
    bsp_uart_irq_handler(BSP_UART1);
}

/**
 * @brief
 * @param
 * @retval
 */
void USART1_IRQHandler(void)
{
    bsp_uart_irq_handler(BSP_UART0);
}

/**
 * @brief
 * @param
 * @retval
 */
void USART2_IRQHandler(void)
{
    bsp_uart_irq_handler(BSP_UART2);
}

void DMA1_Channel4_IRQHandler(void)
{
    bsp_uart_dma_tx_irq_handler(BSP_UART0);
}

void DMA1_Channel7_IRQHandler(void)
{
    bsp_uart_dma_tx_irq_handler(BSP_UART2);
}

void DMA2_Channel2_IRQHandler(void)
{
    bsp_uart_dma_tx_irq_handler(BSP_UART1);
}

/**
 * @brief
 */
/*void EXTI9_5_IRQHandler(void)
{
}*/
void EXTI9_5_IRQHandler(void)
{
    if (RESET != EXTI_GetITStatus(EXTI_LINE5))
    {
        EXTI_ClrITPendBit(EXTI_LINE5);
        bsp_gpio_irq_handler(BSP_GPIO_1);
    }
}

void EXTI0_IRQHandler(void)
{
    if (RESET != EXTI_GetITStatus(EXTI_LINE0))
    {
        EXTI_ClrITPendBit(EXTI_LINE0);
        bsp_gpio_irq_handler(BSP_GPIO_2);
    }
}

void EXTI1_IRQHandler(void)
{
    if (RESET != EXTI_GetITStatus(EXTI_LINE1))
    {
        EXTI_ClrITPendBit(EXTI_LINE1);
        bsp_gpio_irq_handler(BSP_GPIO_0);
    }
}
