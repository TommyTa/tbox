/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_pwm.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include "n32g4fr.h"
#include "n32g4fr_tim.h"
#include "bsp_pwm.h"
#include "errorno.h"

#define PWM_TIM1            (uint32_t)TIM1
#define PWM_TIM2            (uint32_t)TIM2
#define PWM_TIM3            (uint32_t)TIM3
#define PWM_TIM5            (uint32_t)TIM5
#define PWM_TIM8            (uint32_t)TIM8

#define GPIO_A_GROUP        (uint32_t)GPIOA
#define GPIO_B_GROUP        (uint32_t)GPIOB

#define DBG_CTRL            (uint32_t*)0xE0042004
#define _DBG_TIM1_STOP      (1 << 10)
#define _DBG_TIM8_STOP      (1 << 17)

#define PWM_CONFIG_PIN       0
#define PWM_CONFIG_TIM       1
#define PWM_CONFIG_CH        2
#define PWM_CONFIG_GROUP     3

#define PWM_CH1             0
#define PWM_CH2             1
#define PWM_CH3             2
#define PWM_CH4             3

#define APB1_PRES_VALUE     11
#define APB2_PRES_VALUE     4


const static uint32_t pwm_config[10][4] =
{
    { GPIO_PIN_0,   PWM_TIM5, PWM_CH1, GPIO_A_GROUP },  // PA0
    { GPIO_PIN_6,   PWM_TIM3, PWM_CH1, GPIO_A_GROUP },  // PA6      conflict with i2c
    { GPIO_PIN_7,   PWM_TIM8, PWM_CH1, GPIO_A_GROUP },  // PA7
    { GPIO_PIN_8,   PWM_TIM1, PWM_CH1, GPIO_A_GROUP },  // PA8
    { GPIO_PIN_9,   PWM_TIM1, PWM_CH2, GPIO_A_GROUP },  // PA9      conflict with uart
    { GPIO_PIN_10,  PWM_TIM1, PWM_CH3, GPIO_A_GROUP },  // PA10
    { GPIO_PIN_11,  PWM_TIM1, PWM_CH4, GPIO_A_GROUP },  // PA11
    { GPIO_PIN_5,   PWM_TIM3, PWM_CH2, GPIO_B_GROUP },  // PB5_C
    { GPIO_PIN_10,  PWM_TIM2, PWM_CH3, GPIO_B_GROUP },  // PB10_C   conflict with i2c
    { GPIO_PIN_11,  PWM_TIM2, PWM_CH4, GPIO_B_GROUP },  // PB11_C   conflict with i2c
};

static void bsp_pwm_en_dbg_stop(void)
{
    uint32_t* p = DBG_CTRL;

    *p |= DBG_TIM1_STOP;
    *p |= DBG_TIM8_STOP;
}

static void bsp_pwm_gpio_remap(int pin)
{
    if (pwm_config[pin][PWM_CONFIG_TIM] == PWM_TIM1)
    {
        GPIO_ConfigPinRemap(GPIO_ALL_RMP_TIM1, ENABLE);
    }
    else if (pwm_config[pin][PWM_CONFIG_TIM] == PWM_TIM2)
    {
        GPIO_ConfigPinRemap(GPIO_ALL_RMP_TIM2, ENABLE);
    }
    else if (pwm_config[pin][PWM_CONFIG_TIM] == PWM_TIM3)
    {
        GPIO_ConfigPinRemap(GPIO_PART1_RMP_TIM3, ENABLE);
    }
}

static void bsp_pwm_init_cmp(int pin, OCInitType tim_oc_init_st)
{
    uint32_t PWM_CMP = pwm_config[pin][PWM_CONFIG_CH];
    TIM_Module* TIMx = (TIM_Module*)pwm_config[pin][PWM_CONFIG_TIM];

    if (PWM_CMP == PWM_CH1)
    {
        TIM_InitOc1(TIMx, &tim_oc_init_st);
    }
    else if (PWM_CMP == PWM_CH2)
    {
        TIM_InitOc2(TIMx, &tim_oc_init_st);
    }
    else if (PWM_CMP == PWM_CH3)
    {
        TIM_InitOc3(TIMx, &tim_oc_init_st);
    }
    else if (PWM_CMP == PWM_CH4)
    {
        TIM_InitOc4(TIMx, &tim_oc_init_st);
    }
}

void bsp_pwm_init(uint8_t pin, uint8_t val)
{
    uint16_t Channel_Pulse = 0;
    uint16_t PrescalerValue = 0;
    uint16_t timer_period = BSP_PWM_FULL;
    GPIO_InitType GPIO_InitStructure;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    OCInitType TIM_OCInitStructure;
    TIM_Module* TIMx = (TIM_Module*)pwm_config[pin][PWM_CONFIG_TIM];

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2 | RCC_APB1_PERIPH_TIM3 | RCC_APB1_PERIPH_TIM5, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_TIM1 |
    RCC_APB2_PERIPH_AFIO | RCC_APB2_PERIPH_TIM8, ENABLE);

    if (pin >= BSP_PWM_8)
    {
        bsp_pwm_gpio_remap(pin);
    }
    GPIO_InitStructure.Pin        = pwm_config[pin][PWM_CONFIG_PIN];
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitPeripheral((GPIO_Module*)pwm_config[pin][PWM_CONFIG_GROUP], &GPIO_InitStructure);
    if ((TIMx == (TIM_Module*)PWM_TIM1)||(TIMx == (TIM_Module*)PWM_TIM8))
    {
        bsp_pwm_en_dbg_stop();
        PrescalerValue = APB1_PRES_VALUE;
    }
    else
    {
        PrescalerValue = APB2_PRES_VALUE;
    }
    Channel_Pulse = (uint16_t)((val * (timer_period - 1)) / 100);
    TIM_TimeBaseStructure.Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period    = timer_period;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.RepetCnt  = 0;
    TIM_InitTimeBase(TIMx, &TIM_TimeBaseStructure);
    TIM_OCInitStructure.OcMode       = TIM_OCMODE_PWM2;
    TIM_OCInitStructure.OutputState  = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_ENABLE;
    TIM_OCInitStructure.Pulse        = Channel_Pulse;
    TIM_OCInitStructure.OcPolarity   = TIM_OC_POLARITY_LOW;
    TIM_OCInitStructure.OcNPolarity  = TIM_OCN_POLARITY_HIGH;
    TIM_OCInitStructure.OcIdleState  = TIM_OC_IDLE_STATE_SET;
    TIM_OCInitStructure.OcNIdleState = TIM_OC_IDLE_STATE_RESET;
    TIM_Enable(TIMx, ENABLE);
    bsp_pwm_init_cmp(pin, TIM_OCInitStructure);
    if ((TIMx == TIM1)||(TIMx == TIM8))
    {
        TIM_EnableCtrlPwmOutputs(TIMx, ENABLE);
    }
    else
    {
        TIM_ConfigOc2Preload(TIMx, TIM_OC_PRE_LOAD_ENABLE);
    }
}

void bsp_pwm_set(uint8_t pin, uint8_t val)
{
    uint32_t PWM_CMP = pwm_config[pin][PWM_CONFIG_CH];
    TIM_Module* TIMx = (TIM_Module*)pwm_config[pin][PWM_CONFIG_TIM];

    if (PWM_CMP == PWM_CH1)
    {
        TIM_SetCmp1(TIMx, val);
    }
    else if (PWM_CMP == PWM_CH2)
    {
        TIM_SetCmp2(TIMx, val);
    }
    else if (PWM_CMP == PWM_CH3)
    {
        TIM_SetCmp3(TIMx, val);
    }
    else if (PWM_CMP == PWM_CH4)
    {
        TIM_SetCmp4(TIMx, val);
    }
}
