/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ****************************************************************************
 * @file bsp_gpio.c
 * @author tanxt
 * @version v1.0.0
 * ***************************************************************************/

#include <string.h>

#include "n32g4fr.h"
#include "errorno.h"

#include "bsp_gpio.h"
#include "log.h"
bsp_gpio_cb_t gpio_cb[BSP_GPIO_NUM] = {NULL};

typedef struct
{
    uint32_t            rcc;
    GPIO_Module         *gpio;
    uint16_t            pin;
    GPIO_ModeType       mode;

    uint8_t             port_src;
    uint8_t             pin_src;
    IRQn_Type           irq;
    uint8_t             preemption_priority;
    uint8_t             sub_priority;
    uint32_t            exti_line;
    EXTI_TriggerType    trigger;
} bsp_gpio_cfg_t;

bsp_gpio_cfg_t pa1_cfg =
{
    .rcc = RCC_APB2_PERIPH_GPIOA,
    .gpio = GPIOA,
    .pin = GPIO_PIN_1,
    .mode = GPIO_Mode_IPU,

    .port_src = GPIOA_PORT_SOURCE,
    .pin_src = GPIO_PIN_SOURCE1,
    .irq = EXTI1_IRQn,
    .preemption_priority = 4,
    .sub_priority = 0,
    .exti_line = EXTI_LINE1,
    .trigger = EXTI_Trigger_Falling,
};

bsp_gpio_cfg_t pa5_cfg =
{
    .rcc = RCC_APB2_PERIPH_GPIOA,
    .gpio = GPIOA,
    .pin = GPIO_PIN_5,
    .mode = GPIO_Mode_IN_FLOATING,

    .port_src = GPIOA_PORT_SOURCE,
    .pin_src = GPIO_PIN_SOURCE5,
    .irq = EXTI9_5_IRQn,
    .preemption_priority = 8,
    .sub_priority = 0,
    .exti_line = EXTI_LINE5,
    .trigger = EXTI_Trigger_Rising,
};

bsp_gpio_cfg_t pa0_cfg =
{
    .rcc = RCC_APB2_PERIPH_GPIOA,
    .gpio = GPIOA,
    .pin = GPIO_PIN_0,
    .mode = GPIO_Mode_IPU,

    .port_src = GPIOA_PORT_SOURCE,
    .pin_src = GPIO_PIN_SOURCE0,
    .irq = EXTI0_IRQn,
    .preemption_priority = 9,
    .sub_priority = 0,
    .exti_line = EXTI_LINE0,
    .trigger = EXTI_Trigger_Rising_Falling,
};

uint8_t bsp_gpio_read(bsp_gpio_t gpio)
{
    if (gpio == BSP_GPIO_0)
    {
        return GPIO_ReadInputDataBit(GPIOA, GPIO_PIN_1);
    }
    else if (gpio == BSP_GPIO_1)
    {
        return GPIO_ReadInputDataBit(GPIOA, GPIO_PIN_5);
    }
    else if (gpio == BSP_GPIO_2)
    {
        return GPIO_ReadInputDataBit(GPIOA, GPIO_PIN_0);
    }
    else
    {
        return RETVAL(E_STATE);
    }
}

int32_t bsp_gpio_register_irq_callback(bsp_gpio_t gpio, bsp_gpio_cb_t cb)
{
    if (cb == NULL)
    {
        return RETVAL(E_NULL);
    }

    gpio_cb[gpio] = cb;

    return RETVAL(E_OK);
}

void bsp_gpio_irq_handler(bsp_gpio_t gpio)
{
    (*gpio_cb[gpio])();
}

void bsp_gpio_init(bsp_gpio_t gpio, bsp_gpio_exti_cmd_t cmd)
{
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    GPIO_InitType GPIO_InitStructure;

    bsp_gpio_cfg_t *cfg = NULL;

    if (gpio == BSP_GPIO_0)
    {
        cfg = &pa1_cfg;
    }
    else if (gpio == BSP_GPIO_1)
    {
        cfg = &pa5_cfg;
    }
    else if (gpio == BSP_GPIO_2)
    {
        cfg = &pa0_cfg;
    }

    RCC_EnableAPB2PeriphClk(cfg->rcc, ENABLE);
    GPIO_InitStructure.Pin        = cfg->pin;
    GPIO_InitStructure.GPIO_Mode  = cfg->mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitPeripheral(cfg->gpio, &GPIO_InitStructure);

    if (cmd == BSP_GPIO_EXTI_ENABLE)
    {
        GPIO_ConfigEXTILine(cfg->port_src, cfg->pin_src);
        NVIC_InitStructure.NVIC_IRQChannel                   = cfg->irq;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = cfg->preemption_priority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = cfg->sub_priority;
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        EXTI_InitStructure.EXTI_Line    = cfg->exti_line;
        EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = cfg->trigger;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_InitPeripheral(&EXTI_InitStructure);
    }
}


