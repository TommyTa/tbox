/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file led_aw2026.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef __AW2026_H
#define __AW2026_H

/*************reg address******************/
#define AW2026_REGADDR_RSTIDR   0x00
#define AW2026_REGADDR_GCR      0x01
#define AW2026_REGADDR_STATUS   0x02
#define AW2026_REGADDR_IMAX     0x03
#define AW2026_REGADDR_LCFG1    0x04
#define AW2026_REGADDR_LCFG2    0x05
#define AW2026_REGADDR_LCFG3    0x06
#define AW2026_REGADDR_LEDEN    0x07
#define AW2026_REGADDR_LEDCTR   0x08
#define AW2026_REGADDR_PATRUN   0x09
#define AW2026_REGADDR_LED1_1   0x10
#define AW2026_REGADDR_LED2_1   0x11
#define AW2026_REGADDR_LED3_1   0x12
#define AW2026_REGADDR_LED1_2   0x13
#define AW2026_REGADDR_LED2_2   0x14
#define AW2026_REGADDR_LED3_2   0x15
#define AW2026_REGADDR_LED1_3   0x16
#define AW2026_REGADDR_LED2_3   0x17
#define AW2026_REGADDR_LED3_3   0x18
#define AW2026_REGADDR_LED1_4   0x19
#define AW2026_REGADDR_LED2_4   0x1a
#define AW2026_REGADDR_LED3_4   0x1b
#define AW2026_REGADDR_PWM1     0x1c
#define AW2026_REGADDR_PWM2     0x1d
#define AW2026_REGADDR_PWM3     0x1e
#define AW2026_REGADDR_PAT1_T1  0x30
#define AW2026_REGADDR_PAT1_T2  0x31
#define AW2026_REGADDR_PAT1_T3  0x32
#define AW2026_REGADDR_PAT1_T4  0x33
#define AW2026_REGADDR_PAT1_T5  0x34
#define AW2026_REGADDR_PAT2_T1  0x35
#define AW2026_REGADDR_PAT2_T2  0x36
#define AW2026_REGADDR_PAT2_T3  0x37
#define AW2026_REGADDR_PAT2_T4  0x38
#define AW2026_REGADDR_PAT2_T5  0x39
#define AW2026_REGADDR_PAT3_T1  0x3a
#define AW2026_REGADDR_PAT3_T2  0x3b
#define AW2026_REGADDR_PAT3_T3  0x3c
#define AW2026_REGADDR_PAT3_T4  0x3d
#define AW2026_REGADDR_PAT3_T5  0x3e

/*************control data******************/
#define BSP_RGB_EMPTY                       0x00
#define BSP_RGB_ENABLE                      0x07 
#define BSP_RGB_RESET                       0x17
#define BSP_RGB_LEDCTR                      0x03
#define BSP_RGB_DISABLE                     0x01

#define AW2026_LED_OFF                      0x00
#define AW2026_LED1_ON                      0x01
#define AW2026_LED2_ON                      0x02
#define AW2026_LED3_ON                      0x04
#define AW2026_LEDALL_ON                    0x07

#define AW2026_LED1_OUTPUT_CURRENT_VALUE    0x30
#define AW2026_LED2_OUTPUT_CURRENT_VALUE    0x70
#define AW2026_LED3_OUTPUT_CURRENT_VALUE    0xFF

#define AW2026_PATRUN_RUN1                  0x01
#define AW2026_PATRUN_RUN2                  0x02
#define AW2026_PATRUN_RUN3                  0x04
#define AW2026_PATRUN_STOP1                 0x10
#define AW2026_PATRUN_STOP2                 0x20
#define AW2026_PATRUN_STOP3                 0x40

#define AW2026_LCFG_FADE_IN_BIT             (1)
#define AW2026_LCFG_FADE_OUT_BIT            (2)
#define AW2026_PATX_TRISE_BIT               (4)
#define AW2026_PATX_TON_BIT                 (0)
#define AW2026_PATX_TFALL_BIT               (4)
#define AW2026_PATX_TOFF_BIT                (0)

#define AW2026_LED_MANUAL_MODE              0x00

#define DISABLE                             0x00
#define ENABLE                              0x01

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
} led_rgb_mode_step_t;

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
} led_rgb_runtime_t;

#endif
