/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file led.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef __LED_H__
#define __LED_H__

#include "typedefs.h"
#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LED_CHAN_0,
    LED_CHAN_1,
    LED_CHAN_MAX
} led_chan_t;

typedef enum
{
    LED_STATE_ON,
    LED_STATE_OFF,
} led_state_t;

typedef enum
{
    LED_COLOR_BLACK,
    LED_COLOR_WHITE,
    LED_COLOR_YELLOW,
    LED_COLOR_RED,
    LED_COLOR_GREEN,
    LED_COLOR_BLUE,
    LED_COLOR_UNKNOWN,
} led_color_t;

typedef enum
{
    LED_MODE_STEADY,
    LED_MODE_BREATH,
    LED_MODE_UNKNOWN,
} led_mode_t;

typedef struct
{
    led_color_t         color;
    led_mode_t          mode;
    uint16_t            delay;          // The uint is ms.
    uint16_t            trise;          // The uint is ms.Gradually bright
    uint16_t            ton;            // The uint is ms.Steady bright
    uint16_t            tfall;          // The uint is ms.Gradually die
    uint16_t            toff;           // The uint is ms.Shut out the
    uint8_t             bright;
} led_entry_t;

typedef struct
{
    const led_entry_t  *entrys;
    uint8_t             entrys_num;
    uint8_t             repeat_num;     // 0:Forever
} led_pattern_t;

typedef struct led_driver_s led_driver_t;

typedef struct {
    uint8_t chan;
    uint8_t state;
} led_dev_state_t;

typedef struct led_ops_s
{
    int32_t (*init)(void);
    int32_t (*set_pattern)(led_chan_t chan, const led_pattern_t *pattern);
    int32_t (*set_state)(led_chan_t chan, led_state_t state);
    int32_t (*get_state)(led_dev_state_t *dev_state);
} led_ops_t;

struct led_driver_s
{
    driver_t drv;
    led_ops_t ops;
};

int32_t led_driver_register(const char *name, led_driver_t *drv);
led_driver_t* led_driver_find(const char *name);

int32_t led_driver_probe(led_driver_t *drv);
int32_t led_driver_init(led_driver_t *drv);
int32_t led_driver_set_pattern(led_driver_t *drv, led_chan_t chan, const led_pattern_t *pattern);
int32_t led_driver_set_state(led_driver_t *drv, led_chan_t chan, led_state_t state);
int32_t led_driver_get_state(led_driver_t *drv, led_dev_state_t *dev_state);

#ifdef __cplusplus
}
#endif

#endif // __LED_H__
