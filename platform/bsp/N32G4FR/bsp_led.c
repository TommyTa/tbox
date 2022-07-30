/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file led.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include "n32g4fr.h"

#include "bsp_led.h"

/**
 * @brief
 * @param
 * @retval
 */
void bsp_led_init(bsp_led_t led)
{
    if (BSP_LED_0 == led)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
        GPIO_InitType GPIO_InitStructure;
        GPIO_InitStructure.Pin        = GPIO_PIN_5;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
        GPIOB->PBSC = GPIO_PIN_5;
    }
    else if (BSP_LED_1 == led)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
        GPIO_InitType GPIO_InitStructure;
        GPIO_InitStructure.Pin        = GPIO_PIN_8;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
        GPIOA->PBSC = GPIO_PIN_8;
    }
}

/**
 * @brief
 * @param
 * @retval
 */
void bsp_led_on(bsp_led_t led)
{
    if (BSP_LED_0 == led)
    {
        GPIOB->PBSC = GPIO_PIN_5;
    }
    else if (BSP_LED_1 == led)
    {
        GPIOA->PBSC = GPIO_PIN_8;
    }
}

/**
 * @brief
 * @param
 * @retval
 */
void bsp_led_off(bsp_led_t led)
{
    if (BSP_LED_0 == led)
    {
        GPIOB->PBC = GPIO_PIN_5;
    }
    else if (BSP_LED_1 == led)
    {
        GPIOA->PBC = GPIO_PIN_8;
    }
}

void bsp_led_flip(bsp_led_t led)
{
    if (BSP_LED_0 == led)
    {
        GPIOB->POD ^= GPIO_PIN_5;
    }
    else if (BSP_LED_1 == led)
    {
        GPIOA->POD ^= GPIO_PIN_8;
    }
}
