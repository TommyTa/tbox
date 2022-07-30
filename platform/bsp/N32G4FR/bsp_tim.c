#include <stdio.h>
#include "errorno.h"
#include "n32g4fr.h"
#include "bsp_tim.h"
#include "n32g4fr_rcc.h"

static bsp_tim3_cb_t bsp_tim3_cb = NULL;

bsp_tim_cfg bsp_tim3_cfg = {
    .num = 3,
    .timx = TIM3,
    .rcc = RCC_APB1_PERIPH_TIM3,
    .irqn = TIM3_IRQn};

/**
 * @brief 初始化堆栈指针
 * @param timx:定时器结构体  Period:周期  Prescaler:预分频
 * @retval 0--成功  -1--失败
 */
int32_t bsp_tim_init(bsp_tim_cfg timx, uint16_t Period, uint16_t Prescaler)
{
    TIM_TimeBaseInitType timInitStruct;
    NVIC_InitType NVIC_InitStructure;
    if (timx.num < 1 || timx.num > 8)
    {
        return RETVAL(E_FAIL);
    }

    if (timx.num == 1 || timx.num == 8)
    {
        RCC_EnableAPB2PeriphClk(timx.rcc, ENABLE);
    }
    else
    {
        RCC_EnableAPB1PeriphClk(timx.rcc, ENABLE);
    }

    timInitStruct.Period = Period;
    timInitStruct.Prescaler = Prescaler;
    timInitStruct.ClkDiv = 0;
    timInitStruct.CntMode = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(timx.timx, &timInitStruct);

    TIM_ConfigInt(timx.timx, TIM_INT_UPDATE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = timx.irqn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Enable(timx.timx, ENABLE);
    return E_OK;
}

/**
 * @brief 控制TIM使能
 */
void bsp_tim_enable(bsp_tim_cfg timx, FunctionalState Cmd)
{
    TIM_Enable(timx.timx, Cmd);
}

/**
 * @brief 设置TIM3中断回调函数
 */
int32_t bsp_tim3_set_irq_callback(bsp_tim3_cb_t cb)
{
    if (bsp_tim3_cb != NULL)
    {
        return RETVAL(E_FAIL);
    }
    bsp_tim3_cb = cb;
    return E_OK;
}

/**
  * @brief  TIM3中断处理函数每100us。取TIM2->CNT值，计算脉冲个数
            两倍之差。然后将增量脉冲放入最后一个通道的缓冲区，并切换到新通道。
  */
void TIM3_IRQHandler(void)
{
    if (TIM_GetIntStatus(TIM3, TIM_INT_UPDATE) != RESET)
    {
        TIM_ClrIntPendingBit(TIM3, TIM_INT_UPDATE);
        (*bsp_tim3_cb)();
    }
}