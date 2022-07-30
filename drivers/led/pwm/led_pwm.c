/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file led_pwm.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include "log.h"
#include "errorno.h"
#include "modules.h"
#include "sys_cmsis.h"

#include "led.h"
#include "bsp_pwm.h"

#define LED_PWM_LOG_EN 0
#if LED_PWM_LOG_EN
#define LED_PWM_LOG_D(str, ...)     LOG_D(str, ##__VA_ARGS__)
#define LED_PWM_LOG_I(str, ...)     LOG_I(str, ##__VA_ARGS__)
#define LED_PWM_LOG_N(str, ...)     LOG_N(str, ##__VA_ARGS__)
#define LED_PWM_LOG_W(str, ...)     LOG_W(str, ##__VA_ARGS__)
#define LED_PWM_LOG_E(str, ...)     LOG_E(str, ##__VA_ARGS__)
#define LED_PWM_LOG_C(str, ...)     LOG_C(str, ##__VA_ARGS__)
#else
#define LED_PWM_LOG_D(str, ...)
#define LED_PWM_LOG_I(str, ...)
#define LED_PWM_LOG_N(str, ...)
#define LED_PWM_LOG_W(str, ...)
#define LED_PWM_LOG_E(str, ...)
#define LED_PWM_LOG_C(str, ...)
#endif

typedef enum
{
    LED_MODE_STEP_NONE,
    LED_MODE_STEP_FINISH,
    LED_MODE_STEP_STOP,
    LED_MODE_STEP_STEADY_DELAY,
    LED_MODE_STEP_STEADY_ON,
    LED_MODE_STEP_STEADY_OFF,
    LED_MODE_STEP_BREATH_DELAY,
    LED_MODE_STEP_BREATH_BRIGHT,
    LED_MODE_STEP_BREATH_DIM,
} led_pwm_mode_step_t;

typedef struct
{
    uint8_t entrys_num;
    uint8_t entrys_idx;
    uint8_t repeat_num;
    uint8_t repeat_cnt;
    uint8_t mode_type;
    uint8_t mode_step;
    uint8_t bright_peak;
    uint8_t bright_val;
} led_pwm_runtime_t;

const led_pattern_t *g_led_pattern[LED_CHAN_MAX];
static led_state_t g_led_state[LED_CHAN_MAX];
static sys_timer_id_t g_led_pwm_timer_id[LED_CHAN_MAX];
static sys_timer_def_t g_led_pwm_timer[LED_CHAN_MAX];
static led_pwm_runtime_t g_led_pwm_rt[LED_CHAN_MAX];

static void led_pwm_led_ctrl(led_chan_t chan, led_state_t state, uint8_t bright)
{
    uint8_t idx;

    idx = g_led_pwm_rt[chan].entrys_idx;

    if (state == LED_STATE_ON)
    {
        if (g_led_pattern[chan]->entrys[idx].color == LED_COLOR_RED)
        {
            bsp_pwm_set(BSP_PWM_4, bright);
            bsp_pwm_set(BSP_PWM_8, BSP_PWM_EMPTY);
        }
        else
        {
            bsp_pwm_set(BSP_PWM_8, bright);
            bsp_pwm_set(BSP_PWM_4, BSP_PWM_EMPTY);
        }
    }
    else
    {
        bsp_pwm_set(BSP_PWM_4, BSP_PWM_EMPTY);
        bsp_pwm_set(BSP_PWM_8, BSP_PWM_EMPTY);
    }
}

static void led_pwm_runtime_reset(led_chan_t chan)
{
    g_led_pwm_rt[chan].entrys_num = g_led_pattern[chan]->entrys_num;
    g_led_pwm_rt[chan].entrys_idx = 0;
    g_led_pwm_rt[chan].repeat_num = g_led_pattern[chan]->repeat_num;
    g_led_pwm_rt[chan].repeat_cnt = 0;
}

static void led_pwm_runtime_load(led_chan_t chan)
{
    uint8_t idx;

    idx = g_led_pwm_rt[chan].entrys_idx;

    g_led_pwm_rt[chan].mode_type = g_led_pattern[chan]->entrys[idx].mode;
    g_led_pwm_rt[chan].mode_step = LED_MODE_STEP_NONE;

    g_led_pwm_rt[chan].bright_peak = g_led_pattern[chan]->entrys[idx].bright;
    g_led_pwm_rt[chan].bright_val = 0;
}

static void led_pwm_runtime_stop(led_chan_t chan)
{
    g_led_pwm_rt[chan].mode_step = LED_MODE_STEP_STOP;
    led_pwm_led_ctrl(chan, LED_STATE_OFF, 0);
}

static void led_pwm_steady_handle(led_chan_t chan)
{
    led_pwm_runtime_t *p_runtime;
    const led_entry_t *p_entry;

    p_runtime = &(g_led_pwm_rt[chan]);
    p_entry = &(g_led_pattern[chan]->entrys[p_runtime->entrys_idx]);

    switch (p_runtime->mode_step)
    {
        case LED_MODE_STEP_NONE:
        {
            p_runtime->mode_step = LED_MODE_STEP_STEADY_DELAY;
            if (p_entry->delay != 0)
            {
                LED_PWM_LOG_D("[driver][led_pwm][steady] entry[%d] none->delay %d\r\n",
                              p_runtime->entrys_idx, p_entry->delay);
                sys_timer_start(g_led_pwm_timer_id[chan], p_entry->delay);
                break;
            }
        }
        case LED_MODE_STEP_STEADY_DELAY:
        {
            /* Always Off */
            if (p_entry->on_duration == 0)
            {
                LED_PWM_LOG_D("[driver][led_pwm][steady] entry[%d] delay->stop\r\n",
                              p_runtime->entrys_idx);
                p_runtime->mode_step = LED_MODE_STEP_STOP;
            }
            /* Light */
            else
            {
                p_runtime->mode_step = LED_MODE_STEP_STEADY_ON;
                led_pwm_led_ctrl(chan, LED_STATE_ON, p_runtime->bright_peak);
                LED_PWM_LOG_D("[driver][led_pwm][steady] entry[%d] delay->on %d\r\n",
                              p_runtime->entrys_idx, p_entry->on_duration);
                sys_timer_start(g_led_pwm_timer_id[chan], p_entry->on_duration);
            }
        }
        break;
        case LED_MODE_STEP_STEADY_ON:
        {
            p_runtime->mode_step = LED_MODE_STEP_STEADY_OFF;
            if (p_entry->off_duration != 0)
            {
                led_pwm_led_ctrl(chan, LED_STATE_OFF, 0);
                LED_PWM_LOG_D("[driver][led_pwm][steady] entry[%d] on->off %d\r\n",
                              p_runtime->entrys_idx, p_entry->off_duration);
                sys_timer_start(g_led_pwm_timer_id[chan], p_entry->off_duration);
                break;
            }
        }
        case LED_MODE_STEP_STEADY_OFF:
        {
            LED_PWM_LOG_D("[driver][led_pwm][steady] entry[%d] off->finish\r\n",
                          p_runtime->entrys_idx);
            p_runtime->mode_step = LED_MODE_STEP_FINISH;
            sys_timer_start(g_led_pwm_timer_id[chan], 10);
        }
        break;
        default:
        break;
    }
}

static void led_pwm_breath_handle(led_chan_t chan)
{

}

static void led_pwm_pattern_handle(led_chan_t chan)
{
    led_pwm_runtime_t *p_runtime;

    p_runtime = &(g_led_pwm_rt[chan]);

    if (p_runtime->mode_step == LED_MODE_STEP_FINISH)
    {
        p_runtime->entrys_idx ++;
        if (p_runtime->entrys_idx >= p_runtime->entrys_num)
        {
            p_runtime->entrys_idx = 0;

            if (p_runtime->repeat_num == 0)
            {
                LED_PWM_LOG_D("[driver][led_pwm][handle] repeat forever\r\n");
                led_pwm_runtime_load(chan);
            }
            else
            {
                p_runtime->repeat_cnt ++;
                if (p_runtime->repeat_cnt < p_runtime->repeat_num)
                {
                    led_pwm_runtime_load(chan);
                }
                else
                {
                    LED_PWM_LOG_D("[driver][led_pwm][handle] pattern stop\r\n");
                    led_pwm_runtime_stop(chan);
                }
            }
        }
        else
        {
            LED_PWM_LOG_D("[driver][led_pwm][handle] load next entry\r\n");
            led_pwm_runtime_load(chan);
        }
    }

    if (g_led_pwm_rt[chan].mode_type == LED_MODE_STEADY)
    {
        led_pwm_steady_handle(chan);
    }
    else if (g_led_pwm_rt[chan].mode_type == LED_MODE_BREATH)
    {
        led_pwm_breath_handle(chan);
    }
    else
    {
        ;
    }
}

static void led_pwm_chan0_timer_handler(void const *arg)
{
    led_pwm_pattern_handle(LED_CHAN_0);
}

static void led_pwm_chan1_timer_handler(void const *arg)
{
    led_pwm_pattern_handle(LED_CHAN_1);
}

static int32_t led_pwm_init(void)
{
    g_led_pattern[0] = NULL;
    g_led_pattern[1] = NULL;

    g_led_state[0] = LED_STATE_OFF;
    g_led_state[1] = LED_STATE_OFF;

    g_led_pwm_timer[0].ptimer = led_pwm_chan0_timer_handler;
    g_led_pwm_timer[1].ptimer = led_pwm_chan1_timer_handler;

    g_led_pwm_timer_id[0] = sys_timer_create(&g_led_pwm_timer[0], SYS_TIMER_ONCE, NULL);
    if (!g_led_pwm_timer_id[0])
    {
        return RETVAL(E_FAIL);
    }
    g_led_pwm_timer_id[1] = sys_timer_create(&g_led_pwm_timer[1], SYS_TIMER_ONCE, NULL);
    if (!g_led_pwm_timer_id[1])
    {
        return RETVAL(E_FAIL);
    }

    bsp_pwm_init(BSP_PWM_4, BSP_PWM_EMPTY);
    bsp_pwm_init(BSP_PWM_8, BSP_PWM_EMPTY);

    return RETVAL(E_OK);
}

static int32_t led_pwm_set_pattern(led_chan_t chan, const led_pattern_t *pattern)
{
    sys_timer_stop(g_led_pwm_timer_id[chan]);

    g_led_pattern[chan] = pattern;

    led_pwm_runtime_stop(chan);
    led_pwm_runtime_reset(chan);
    led_pwm_runtime_load(chan);
    led_pwm_pattern_handle(chan);

    return RETVAL(E_OK);
}

static int32_t led_pwm_set_state(led_chan_t chan, led_state_t state)
{
    return RETVAL(E_OK);
}

static int32_t led_pwm_probe(void)
{
    return RETVAL(E_OK);
}

static int32_t led_pwm_remove(void)
{
    return RETVAL(E_OK);
}

static int32_t led_pwm_shutdown(driver_t *drv)
{
    return RETVAL(E_OK);
}

static int32_t led_pwm_suspend(driver_t *drv)
{
    return RETVAL(E_OK);
}

static int32_t led_pwm_resume(driver_t *drv)
{
    return RETVAL(E_OK);
}

static driver_pm_ops_t led_pwm_pm = {
    .shutdown = led_pwm_shutdown,
    .suspend = led_pwm_suspend,
    .resume = led_pwm_resume,
};

static led_driver_t led_pwm = {
    .drv = {
        .probe = led_pwm_probe,
        .remove = led_pwm_remove,
        .pm_ops = &led_pwm_pm,
    },
    .ops = {
        .init = led_pwm_init,
        .set_pattern = led_pwm_set_pattern,
        .set_state = led_pwm_set_state,
    }
};

static void led_pwm_register(void)
{
    int32_t ret;

    ret = led_driver_register("led-pwm", &led_pwm);
    if (ret != RETVAL(E_OK))
    {
        ;
    }
}

DRIVER_INITCALL("led-pwm", led_pwm_register);
