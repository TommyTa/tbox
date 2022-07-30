/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ****************************************************************************
 * @file key.c
 * @author tanxt
 * @version v1.0.0
 * ***************************************************************************/

#ifndef __KEY_H__
#define __KEY_H__

#include "typedefs.h"

typedef enum
{
    KEY0_POWER,
    KEY1_HALL,
    KEY2_USB,
    KEY_NUM,
} key_num_t;

#define KEY_SAMPLE_INTERVAL_MS          10

typedef enum
{
    KEY_EVENT_NONE              = 0,
    KEY_EVENT_PRESS             = (1 << 0),
    KEY_EVENT_RELEASE           = (1 << 1),
    KEY_EVENT_LONG_HOLD         = (1 << 2),
    KEY_EVENT_LONG_RELEASE      = (1 << 3),
    KEY_EVENT_LONG_LONG_HOLD    = (1 << 4),
    KEY_EVENT_LONG_LONG_RELEASE = (1 << 5),
    KEY_EVENT_CLICK             = (1 << 6),
    KEY_EVENT_DOUBLE_CLICK      = (1 << 7),
    KEY_EVENT_TRIPLE_CLICK      = (1 << 8),
} key_event_t;

typedef enum
{
    KEY_TRIGGER_MODE_PULSE,
    KEY_TRIGGER_MODE_LEVEL,
} key_trigger_t;

typedef struct
{
    uint8_t active_level;
    uint8_t debounce_time;
    uint16_t effective_interval;
    key_trigger_t trigger;
} key_cfg_t;

typedef void (*key_event_cb_t)(key_num_t key, uint32_t event);

void key_init(key_num_t key, key_cfg_t *cfg);
int32_t key_event_register_callback(key_num_t key, key_event_cb_t cb);

#endif //__KEY_H__
