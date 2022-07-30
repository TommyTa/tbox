/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file led_aw2026.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include <stdlib.h>
#include "log.h"
#include "errorno.h"
#include "modules.h"
#include "sys_cmsis.h"

#include "led.h"
#include "bsp_i2c.h"
#include "aw2026.h"

#define LED_RGB_LOG_EN 0
#if LED_RGB_LOG_EN
#define LED_RGB_LOG_D(str, ...)     LOG_D(str, ##__VA_ARGS__)
#define LED_RGB_LOG_I(str, ...)     LOG_I(str, ##__VA_ARGS__)
#define LED_RGB_LOG_N(str, ...)     LOG_N(str, ##__VA_ARGS__)
#define LED_RGB_LOG_W(str, ...)     LOG_W(str, ##__VA_ARGS__)
#define LED_RGB_LOG_E(str, ...)     LOG_E(str, ##__VA_ARGS__)
#define LED_RGB_LOG_C(str, ...)     LOG_C(str, ##__VA_ARGS__)
#else
#define LED_RGB_LOG_D(str, ...)
#define LED_RGB_LOG_I(str, ...)
#define LED_RGB_LOG_N(str, ...)
#define LED_RGB_LOG_W(str, ...)
#define LED_RGB_LOG_E(str, ...)
#define LED_RGB_LOG_C(str, ...)
#endif

#define DEV_NAME                            "led-aw20x"
#define AW2026_SLAVE_ADDR                   0x64
#define AW2026_CHIP_ID                      0x31

static const led_pattern_t  *g_led_pattern[LED_CHAN_MAX];
static led_state_t           g_led_state[LED_CHAN_MAX];
static sys_timer_id_t        g_led_rgb_timer_id[LED_CHAN_MAX];
static sys_timer_def_t       g_led_rgb_timer[LED_CHAN_MAX];
static led_rgb_runtime_t     g_led_rgb_rt[LED_CHAN_MAX];

const static int16_t g_aw2026_time_pattern_ms[] = {0, 130, 260, 380, 510, 770, 1040, 1600,
                                                   2100, 2600, 3100, 4200, 5200, 6200, 7300, 8300};

static int32_t aw2026_reg_write(led_chan_t chan, uint8_t reg, uint8_t val)
{
    int32_t ret;
    i2c_transfer_t i2c;

    i2c.bus = chan;
    i2c.slave_addr = AW2026_SLAVE_ADDR;

    ret = bsp_i2c_write(&i2c, reg, &val, 1);
    if (ret != RETVAL(E_OK))
    {
        LED_RGB_LOG_D("[aw2026][aw2026_reg_write] i2c write buf failed !!!\r\r\n");
        return RETVAL(E_FAIL);
    }
    return ret;
}

static int32_t aw2026_reg_read(led_chan_t chan, uint8_t reg, uint8_t *buf)
{
    int32_t ret;
    i2c_transfer_t i2c;

    i2c.bus = chan;
    i2c.slave_addr = AW2026_SLAVE_ADDR;

    ret = bsp_i2c_read(&i2c, reg, buf, 1);
    if (ret != RETVAL(E_OK))
    {
        LED_RGB_LOG_D("[aw2026][aw2026_reg_write] i2c write buf failed !!!\r\r\n");
        return RETVAL(E_FAIL);
    }
    return ret;
}

static uint8_t aw2026_tims_to_reg_val(uint16_t input)
{
    int16_t temp = 0x2FFF;
    uint8_t location = 0;

    for (uint8_t i = 0; i < sizeof(g_aw2026_time_pattern_ms) / sizeof(int32_t); i++)
    {
        if (abs(g_aw2026_time_pattern_ms[i] - input) < temp)
        {
            temp = abs(g_aw2026_time_pattern_ms[i] - input);
            location = i;
        }
    }
    return location;
}

static void led_set_reg_bit(led_chan_t chan, uint8_t reg, uint8_t bit, uint8_t en)
{
    uint8_t byte;

    aw2026_reg_read(chan, reg, &byte);
    if (en)
    {
        byte |= (1 << bit);
    }
    else
    {
        byte &= ~(1 << bit);
    }
    aw2026_reg_write(chan, reg, byte);
}

static void led_bright_green(led_chan_t chan, uint8_t bright)
{
    aw2026_reg_write(chan, AW2026_REGADDR_PWM1, bright);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM2, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM3, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_LEDEN, AW2026_LED1_ON);
}

static void led_bright_red(led_chan_t chan, uint8_t bright)
{
    aw2026_reg_write(chan, AW2026_REGADDR_PWM1, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM2, bright);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM3, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_LEDEN, AW2026_LED2_ON);
}

static void led_bright_white(led_chan_t chan, uint8_t bright)
{
    aw2026_reg_write(chan, AW2026_REGADDR_PWM1, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM2, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM3, bright);
    aw2026_reg_write(chan, AW2026_REGADDR_LEDEN, AW2026_LED3_ON);
}

static void led_bright_yellow(led_chan_t chan, uint8_t bright)
{
    aw2026_reg_write(chan, AW2026_REGADDR_PWM1, bright);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM2, bright);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM3, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_LEDEN, AW2026_LEDALL_ON);
}

static void led_breath_green(led_chan_t chan, uint8_t bright)
{
    led_bright_green(chan, bright);
}

static void led_breath_red(led_chan_t chan, uint8_t bright)
{
    led_bright_red(chan, bright);
}

static void led_breath_white(led_chan_t chan, uint8_t bright)
{
    led_bright_white(chan, bright);
}

static void led_breath_yellow(led_chan_t chan, uint8_t bright)
{
    led_bright_yellow(chan, bright);
}

static void led_all_off(led_chan_t chan)
{
    aw2026_reg_write(chan, AW2026_REGADDR_PWM1, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM2, AW2026_LED_OFF);
    aw2026_reg_write(chan, AW2026_REGADDR_PWM3, AW2026_LED_OFF);
}

static void led_all_stop(led_chan_t chan)
{
    aw2026_reg_write(chan, AW2026_REGADDR_LEDEN, AW2026_LED_OFF);
}

static void led_set_pattern_time(led_chan_t chan, uint8_t reg, uint8_t bit, uint16_t time)
{
    uint8_t val = 0;
    uint8_t byte = 0;

    val = aw2026_tims_to_reg_val(time);
    aw2026_reg_read(chan, reg, &byte);
    if (bit >= 4)
    {
        // reset high 4 bit
        byte &= (uint8_t)(0x0F);
    }
    else
    {
        // reset low 4 bit
        byte &= (uint8_t)(0xF0);
    }
    byte |= (uint8_t)(val << bit);
    aw2026_reg_write(chan, reg, byte);
}

static void led_set_cycle_time(led_chan_t chan, led_entry_t led_entry)
{
    led_color_t color;
    uint8_t val;

    color = led_entry.color;
    switch (color)
    {
        case LED_COLOR_RED:
            val = (led_entry.trise > 0) ? ENABLE : DISABLE;
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG2, AW2026_LCFG_FADE_IN_BIT, val);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT2_T1, AW2026_PATX_TRISE_BIT, led_entry.trise);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT2_T1, AW2026_PATX_TON_BIT, led_entry.ton);
            val = (led_entry.tfall > 0) ? ENABLE : DISABLE;
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG2, AW2026_LCFG_FADE_OUT_BIT, val);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT2_T2, AW2026_PATX_TFALL_BIT, led_entry.tfall);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT2_T2, AW2026_PATX_TOFF_BIT, led_entry.toff);
            break;
        case LED_COLOR_GREEN:
            val = (led_entry.trise > 0) ? ENABLE : DISABLE;
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG1, AW2026_LCFG_FADE_IN_BIT, val);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT1_T1, AW2026_PATX_TRISE_BIT, led_entry.trise);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT1_T1, AW2026_PATX_TON_BIT, led_entry.ton);
            val = (led_entry.tfall > 0) ? ENABLE : DISABLE;
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG1, AW2026_LCFG_FADE_OUT_BIT, val);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT1_T2, AW2026_PATX_TFALL_BIT, led_entry.tfall);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT1_T2, AW2026_PATX_TOFF_BIT, led_entry.toff);
            break;
        case LED_COLOR_WHITE:
            val = (led_entry.trise > 0) ? ENABLE : DISABLE;
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG3, AW2026_LCFG_FADE_IN_BIT, val);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT3_T1, AW2026_PATX_TRISE_BIT, led_entry.trise);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT3_T1, AW2026_PATX_TON_BIT, led_entry.ton);
            val = (led_entry.tfall > 0) ? ENABLE : DISABLE;
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG3, AW2026_LCFG_FADE_OUT_BIT, val);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT3_T2, AW2026_PATX_TFALL_BIT, led_entry.tfall);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT3_T2, AW2026_PATX_TOFF_BIT, led_entry.toff);
            break;
        case LED_COLOR_YELLOW:
            val = (led_entry.trise > 0) ? ENABLE : DISABLE;
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG1, AW2026_LCFG_FADE_IN_BIT, val);
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG2, AW2026_LCFG_FADE_IN_BIT, val);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT1_T1, AW2026_PATX_TRISE_BIT, led_entry.trise);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT2_T1, AW2026_PATX_TRISE_BIT, led_entry.trise);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT1_T1, AW2026_PATX_TON_BIT, led_entry.ton);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT2_T1, AW2026_PATX_TON_BIT, led_entry.ton);
            val = (led_entry.tfall > 0) ? ENABLE : DISABLE;
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG1, AW2026_LCFG_FADE_OUT_BIT, val);
            led_set_reg_bit(chan, AW2026_REGADDR_LCFG2, AW2026_LCFG_FADE_OUT_BIT, val);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT1_T2, AW2026_PATX_TFALL_BIT, led_entry.tfall);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT2_T2, AW2026_PATX_TFALL_BIT, led_entry.tfall);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT1_T2, AW2026_PATX_TOFF_BIT, led_entry.toff);
            led_set_pattern_time(chan, AW2026_REGADDR_PAT2_T2, AW2026_PATX_TOFF_BIT, led_entry.toff);
            break;
        default:
            break;
    }
}

static int32_t aw2026_init(led_chan_t chan)
{
    uint8_t chip_id;

    aw2026_reg_read(chan, AW2026_REGADDR_RSTIDR, &chip_id);
    if(chip_id != AW2026_CHIP_ID)
    {
        LED_RGB_LOG_D("chip_id is 0x%02x\r\n", chip_id);
        return RETVAL(E_FAIL);
    }

    aw2026_reg_write(chan, AW2026_REGADDR_GCR,    BSP_RGB_ENABLE);
    aw2026_reg_write(chan, AW2026_REGADDR_LEDEN,  BSP_RGB_ENABLE);
    aw2026_reg_write(chan, AW2026_REGADDR_LEDCTR, AW2026_LED_MANUAL_MODE);

    aw2026_reg_write(chan, AW2026_REGADDR_LED1_1, AW2026_LED1_OUTPUT_CURRENT_VALUE);
    aw2026_reg_write(chan, AW2026_REGADDR_LED2_1, AW2026_LED2_OUTPUT_CURRENT_VALUE);
    aw2026_reg_write(chan, AW2026_REGADDR_LED3_1, AW2026_LED3_OUTPUT_CURRENT_VALUE);

    aw2026_reg_write(chan, AW2026_REGADDR_LCFG1, AW2026_LED_MANUAL_MODE);
    aw2026_reg_write(chan, AW2026_REGADDR_LCFG2, AW2026_LED_MANUAL_MODE);
    aw2026_reg_write(chan, AW2026_REGADDR_LCFG3, AW2026_LED_MANUAL_MODE);

    led_all_stop(chan);

    return RETVAL(E_OK);
}

static void led_rgb_mode_bright(led_chan_t chan, led_entry_t led_entry, uint8_t bright)
{
    led_color_t color;

    color = led_entry.color;

    if (color == LED_COLOR_RED)
    {
        led_bright_red(chan, bright);
    }
    else if (color == LED_COLOR_WHITE)
    {
        led_bright_white(chan, bright);
    }
    else if (color == LED_COLOR_GREEN)
    {
        led_bright_green(chan, bright);
    }
    else if (color == LED_COLOR_YELLOW)
    {
        led_bright_yellow(chan, bright);
    }
    else
    {
        led_all_off(chan);
    }
}

static void led_rgb_mode_breath(led_chan_t chan, led_entry_t led_entry, uint8_t bright)
{
    led_color_t color;

    color = led_entry.color;
    if (color == LED_COLOR_RED)
    {
        led_breath_red(chan, bright);
    }
    else if (color == LED_COLOR_WHITE)
    {
        led_breath_white(chan, bright);
    }
    else if (color == LED_COLOR_GREEN)
    {
        led_breath_green(chan, bright);
    }
    else if (color == LED_COLOR_YELLOW)
    {
        led_breath_yellow(chan, bright);
    }
    else
    {
        led_all_off(chan);
    }

}

static void led_rgb_led_ctrl(led_chan_t chan, led_state_t state, uint8_t bright)
{
    uint8_t idx;
    led_mode_t mode;

    idx = g_led_rgb_rt[chan].entrys_idx;
    mode = g_led_pattern[chan]->entrys[idx].mode;

    LED_RGB_LOG_D("led_rgb_set bright is 0x%02x, state:%d, mode:%d\r\n",
                 bright, state, mode);

    if (state == LED_STATE_ON)
    {
        if(mode == LED_MODE_STEADY)
        {
            led_rgb_mode_bright(chan, g_led_pattern[chan]->entrys[idx], bright);
        }
        else if(LED_MODE_BREATH)
        {
            led_rgb_mode_breath(chan, g_led_pattern[chan]->entrys[idx], bright);
        }
    }
    else
    {
        led_all_off(chan);
    }
}

static void led_rgb_runtime_reset(led_chan_t chan)
{
    g_led_rgb_rt[chan].entrys_num = g_led_pattern[chan]->entrys_num;
    g_led_rgb_rt[chan].entrys_idx = 0;
    g_led_rgb_rt[chan].repeat_num = g_led_pattern[chan]->repeat_num;
    g_led_rgb_rt[chan].repeat_cnt = 0;
}

static void led_rgb_runtime_load(led_chan_t chan)
{
    uint8_t idx;

    idx = g_led_rgb_rt[chan].entrys_idx;

    g_led_rgb_rt[chan].mode_type = g_led_pattern[chan]->entrys[idx].mode;
    g_led_rgb_rt[chan].mode_step = LED_MODE_STEP_NONE;

    g_led_rgb_rt[chan].bright_peak = g_led_pattern[chan]->entrys[idx].bright;
    g_led_rgb_rt[chan].bright_val = 0;

    led_set_cycle_time(chan, g_led_pattern[chan]->entrys[idx]);
}

static void led_rgb_runtime_stop(led_chan_t chan)
{
    g_led_rgb_rt[chan].mode_step = LED_MODE_STEP_STOP;
    led_rgb_led_ctrl(chan, LED_STATE_OFF, 0);
}

static void led_rgb_steady_handle(led_chan_t chan)
{
    led_rgb_runtime_t *p_runtime;
    const led_entry_t *p_entry;

    p_runtime = &(g_led_rgb_rt[chan]);
    p_entry = &(g_led_pattern[chan]->entrys[p_runtime->entrys_idx]);

    switch (p_runtime->mode_step)
    {
        case LED_MODE_STEP_NONE:
        {
            p_runtime->mode_step = LED_MODE_STEP_STEADY_DELAY;
            if (p_entry->delay != 0)
            {
                LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] none->delay %d\r\n",
                              p_runtime->entrys_idx, p_entry->delay);
                sys_timer_start(g_led_rgb_timer_id[chan], p_entry->delay);
                break;
            }
        }
        case LED_MODE_STEP_STEADY_DELAY:
        {
            /* Always Off */
            if (p_entry->ton == 0)
            {
                LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] delay->stop\r\n",
                              p_runtime->entrys_idx);
                p_runtime->mode_step = LED_MODE_STEP_STOP;
            }
            /* Light */
            else
            {
                p_runtime->mode_step = LED_MODE_STEP_STEADY_ON;
                led_rgb_led_ctrl(chan, LED_STATE_ON, p_runtime->bright_peak);
                LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] delay->on %d\r\n",
                              p_runtime->entrys_idx, p_entry->ton);
                sys_timer_start(g_led_rgb_timer_id[chan], p_entry->ton);
            }
        }
        break;
        case LED_MODE_STEP_STEADY_ON:
        {
            p_runtime->mode_step = LED_MODE_STEP_STEADY_OFF;
            if (p_entry->toff != 0)
            {
                led_rgb_led_ctrl(chan, LED_STATE_OFF, 0);
                LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] on->off %d\r\n",
                              p_runtime->entrys_idx, p_entry->toff);
                sys_timer_start(g_led_rgb_timer_id[chan], p_entry->toff);
                break;
            }
        }
        case LED_MODE_STEP_STEADY_OFF:
        {
            LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] off->finish\r\n",
                          p_runtime->entrys_idx);
            p_runtime->mode_step = LED_MODE_STEP_FINISH;
            sys_timer_start(g_led_rgb_timer_id[chan], 10);
        }
        break;
        default:
        break;
    }
}

static void led_rgb_breath_handle(led_chan_t chan)
{
    led_rgb_runtime_t *p_runtime;
    const led_entry_t *p_entry;

    p_runtime = &(g_led_rgb_rt[chan]);
    p_entry = &(g_led_pattern[chan]->entrys[p_runtime->entrys_idx]);

    switch (p_runtime->mode_step)
    {
        case LED_MODE_STEP_NONE:
        {
            p_runtime->mode_step = LED_MODE_STEP_STEADY_DELAY;
            if (p_entry->delay != 0)
            {
                LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] none->delay %d\r\n",
                              p_runtime->entrys_idx, p_entry->delay);
                sys_timer_start(g_led_rgb_timer_id[chan], p_entry->delay);
                break;
            }
        }
        case LED_MODE_STEP_STEADY_DELAY:
        {
            /* Always Off */
            if (p_entry->ton == 0)
            {
                LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] delay->stop\r\n",
                              p_runtime->entrys_idx);
                p_runtime->mode_step = LED_MODE_STEP_STOP;
            }
            /* Light */
            else
            {
                p_runtime->mode_step = LED_MODE_STEP_STEADY_ON;
                led_rgb_led_ctrl(chan, LED_STATE_ON, p_runtime->bright_peak);
                LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] delay->on %d\r\n",
                              p_runtime->entrys_idx, p_entry->ton);
                sys_timer_start(g_led_rgb_timer_id[chan], p_entry->ton);
            }
        }
        break;
        case LED_MODE_STEP_STEADY_ON:
        {
            p_runtime->mode_step = LED_MODE_STEP_STEADY_OFF;
            if (p_entry->toff != 0)
            {
                led_rgb_led_ctrl(chan, LED_STATE_OFF, 0);
                LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] on->off %d\r\n",
                              p_runtime->entrys_idx, p_entry->toff);
                sys_timer_start(g_led_rgb_timer_id[chan], p_entry->toff);
                break;
            }
        }
        case LED_MODE_STEP_STEADY_OFF:
        {
            LED_RGB_LOG_D("[driver][led_rgb][steady] entry[%d] off->finish\r\n",
                          p_runtime->entrys_idx);
            p_runtime->mode_step = LED_MODE_STEP_FINISH;
            sys_timer_start(g_led_rgb_timer_id[chan], 10);
        }
        break;
        default:
        break;
    }
}

static void led_rgb_pattern_handle(led_chan_t chan)
{
    led_rgb_runtime_t *p_runtime;

    p_runtime = &(g_led_rgb_rt[chan]);

    if (p_runtime->mode_step == LED_MODE_STEP_FINISH)
    {
        p_runtime->entrys_idx ++;
        if (p_runtime->entrys_idx >= p_runtime->entrys_num)
        {
            p_runtime->entrys_idx = 0;

            if (p_runtime->repeat_num == 0)
            {
                LED_RGB_LOG_D("[driver][led_rgb][handle] repeat forever\r\n");
                led_rgb_runtime_load(chan);
            }
            else
            {
                p_runtime->repeat_cnt ++;
                if (p_runtime->repeat_cnt < p_runtime->repeat_num)
                {
                    g_led_state[chan] = LED_STATE_ON;
                    led_rgb_runtime_load(chan);
                }
                else
                {
                    g_led_state[chan] = LED_STATE_OFF;
                    led_rgb_runtime_stop(chan);
                    LED_RGB_LOG_D("[driver][led_rgb][handle] pattern stop\r\n");
                }
            }
        }
        else
        {
            LED_RGB_LOG_D("[driver][led_rgb][handle] load next entry\r\n");
            led_rgb_runtime_load(chan);
        }
    }

    if (g_led_rgb_rt[chan].mode_type == LED_MODE_STEADY)
    {
        led_rgb_steady_handle(chan);
    }
    else if (g_led_rgb_rt[chan].mode_type == LED_MODE_BREATH)
    {
        led_rgb_breath_handle(chan);
    }
    else
    {
        ;
    }
}

static void led_rgb_chan0_timer_handler(void const *arg)
{
    led_rgb_pattern_handle(LED_CHAN_0);
}

static void led_rgb_chan1_timer_handler(void const *arg)
{
    led_rgb_pattern_handle(LED_CHAN_1);
}

static int32_t led_rgb_init(void)
{
    int32_t ret, ret1;

    ret = aw2026_init(LED_CHAN_0);
    ret1 = aw2026_init(LED_CHAN_1);
    if (ret != RETVAL(E_OK) && ret1 != RETVAL(E_OK))
    {
        return RETVAL(E_FAIL);
    }

    g_led_pattern[0] = NULL;
    g_led_pattern[1] = NULL;

    g_led_state[0] = LED_STATE_OFF;
    g_led_state[1] = LED_STATE_OFF;

    g_led_rgb_timer[0].ptimer = led_rgb_chan0_timer_handler;
    g_led_rgb_timer[1].ptimer = led_rgb_chan1_timer_handler;

    g_led_rgb_timer_id[0] = sys_timer_create(&g_led_rgb_timer[0], SYS_TIMER_ONCE, NULL);
    if (!g_led_rgb_timer_id[0])
    {
        return RETVAL(E_FAIL);
    }
    g_led_rgb_timer_id[1] = sys_timer_create(&g_led_rgb_timer[1], SYS_TIMER_ONCE, NULL);
    if (!g_led_rgb_timer_id[1])
    {
        return RETVAL(E_FAIL);
    }

    return RETVAL(E_OK);
}

static int32_t led_rgb_set_pattern(led_chan_t chan, const led_pattern_t *pattern)
{
    sys_timer_stop(g_led_rgb_timer_id[chan]);

    g_led_pattern[chan] = pattern;

    led_rgb_runtime_stop(chan);
    led_rgb_runtime_reset(chan);
    led_rgb_runtime_load(chan);
    led_rgb_pattern_handle(chan);

    return RETVAL(E_OK);
}

static int32_t led_rgb_set_state(led_chan_t chan, led_state_t state)
{
    return RETVAL(E_OK);
}

static int32_t led_rgb_get_state(led_dev_state_t *led_dev)
{
    if (led_dev->chan > LED_CHAN_MAX)
    {
        return RETVAL(E_FAIL);
    }
    led_dev->state = g_led_state[led_dev->chan];
    LED_RGB_LOG_D("STATUS:%d\r\n", g_led_state[led_dev->chan]);

    return RETVAL(E_OK);
}

static int32_t led_rgb_probe(void)
{
    return RETVAL(E_OK);
}

static int32_t led_rgb_remove(void)
{
    return RETVAL(E_OK);
}

static int32_t led_rgb_shutdown(driver_t *drv)
{
    return RETVAL(E_OK);
}

static int32_t led_rgb_suspend(driver_t *drv)
{
    return RETVAL(E_OK);
}

static int32_t led_rgb_resume(driver_t *drv)
{
    return RETVAL(E_OK);
}

static driver_pm_ops_t led_rgb_pm = {
    .shutdown = led_rgb_shutdown,
    .suspend = led_rgb_suspend,
    .resume = led_rgb_resume,
};

static led_driver_t led_rgb = {
    .drv = {
        .probe = led_rgb_probe,
        .remove = led_rgb_remove,
        .pm_ops = &led_rgb_pm,
    },
    .ops = {
        .init = led_rgb_init,
        .set_pattern = led_rgb_set_pattern,
        .set_state = led_rgb_set_state,
        .get_state = led_rgb_get_state,
    }
};

static void led_rgb_register(void)
{
    int32_t ret;

    ret = led_driver_register(DEV_NAME, &led_rgb);
    if (ret != RETVAL(E_OK))
    {
        LED_RGB_LOG_D("aw2026 register fail.\r\n");
    }
}

DRIVER_INITCALL(DEV_NAME, led_rgb_register);
