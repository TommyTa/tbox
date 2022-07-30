/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file ui_event.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdint.h>
#include <string.h>

#include "sys_cmsis.h"
#include "cmsis_os.h"
#include "modules.h"
#include "errorno.h"
#include "key.h"
#include "ux.h"

#include "log.h"

typedef enum
{
    STATE_POWER_OFF = 0,
    STATE_POWER_ON,
} state_t;

typedef enum
{
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH,
} gpio_level_t;

typedef void (*ui_event_action_t)(void);

typedef struct
{
    state_t state;
    key_num_t key;
    key_event_t event;
    ui_event_action_t action;
} ui_event_action_table_t;

state_t g_state = STATE_POWER_ON;

static key_cfg_t ui_event_cfg[KEY_NUM] =
{
    /* Power Key */
    {
        .active_level       = GPIO_LEVEL_LOW,
        .debounce_time      = 2 * KEY_SAMPLE_INTERVAL_MS,
        .effective_interval = 5 * KEY_SAMPLE_INTERVAL_MS,
        .trigger            = KEY_TRIGGER_MODE_LEVEL,
    },
    /* Cover Hall */
    {
        .active_level       = GPIO_LEVEL_HIGH,
        .debounce_time      = 2 * KEY_SAMPLE_INTERVAL_MS,
        .effective_interval = 5 * KEY_SAMPLE_INTERVAL_MS,
        .trigger            = KEY_TRIGGER_MODE_LEVEL,
    },
    /* USB Cable */
    {
        .active_level       = GPIO_LEVEL_LOW,
        .debounce_time      = 0 * KEY_SAMPLE_INTERVAL_MS,
        .effective_interval = 50 * KEY_SAMPLE_INTERVAL_MS,
        .trigger            = KEY_TRIGGER_MODE_PULSE,
    },
};

static void ui_event_box_long(void)
{
    ux_event_put_signal(UX_SIGNAL_BOX_LONG);
}

static void ui_event_box_long_long(void)
{
    ux_event_put_signal(UX_SIGNAL_BOX_LONG_LONG);
}

static void ui_event_box_five_click(void)
{
    ux_event_put_signal(UX_SIGNAL_BOX_FIVE_CLICK);
}

static void ui_event_box_open(void)
{
    ux_event_put_signal(UX_SIGNAL_BOX_OPEN);
}

static void ui_event_box_close(void)
{
    ux_event_put_signal(UX_SIGNAL_BOX_CLOSE);
}

static void ui_event_charge_change(void)
{
    ;
}

static ui_event_action_table_t ui_event_action_table[] =
{
    { STATE_POWER_ON,   KEY0_POWER,     KEY_EVENT_LONG_HOLD,            ui_event_box_long },
    { STATE_POWER_ON,   KEY0_POWER,     KEY_EVENT_LONG_LONG_HOLD,       ui_event_box_long_long },
    { STATE_POWER_ON,   KEY0_POWER,     KEY_EVENT_TRIPLE_CLICK,         ui_event_box_five_click },
    { STATE_POWER_ON,   KEY1_HALL,      KEY_EVENT_PRESS,                ui_event_box_open },
    { STATE_POWER_ON,   KEY1_HALL,      KEY_EVENT_RELEASE,              ui_event_box_close },
    { STATE_POWER_ON,   KEY2_USB,       KEY_EVENT_PRESS,                ui_event_charge_change },
};

static void ui_event_action_process(key_num_t key, key_event_t event)
{
    uint8_t i;

    for (i = 0; i < ARRAY_SIZE(ui_event_action_table); i++)
    {
        if ((event == ui_event_action_table[i].event) && (key == ui_event_action_table[i].key))
        {
            if (g_state == ui_event_action_table[i].state)
            {
                (*ui_event_action_table[i].action)();
            }
        }
    }
}

typedef struct
{
    key_num_t key;
    uint32_t event;
} ui_event_msg_t;

SYS_MSG_QDEF(ui_event_msg, 5, uint32_t);
static sys_msg_id_t ui_event_msg_id;

static void ui_event_task(void const *argument)
{
    sys_event_t evt;
    ui_event_msg_t *p_msg = NULL;
    key_event_t event;
    key_num_t key;

    while(1)
    {
        evt = sys_msg_get(ui_event_msg_id, SYS_WAIT_FOREVER);

        if (evt.status == SYS_EVENT_MESSAGE)
        {
            p_msg = (ui_event_msg_t*)evt.value.v;

            key = p_msg->key;
            event = p_msg->event;
            mem_free(p_msg);

            ui_event_action_process(key, event);
        }
    }
}

SYS_THREAD_DEF(ui_event_task, SYS_PRIORITY_BELOW_NORMAL, 200);

static void ui_event_handle(key_num_t key, uint32_t event)
{
    ui_event_msg_t *p_msg = NULL;

    p_msg = mem_malloc(sizeof(ui_event_msg_t));

    if (p_msg != NULL)
    {
        p_msg->key = key;
        p_msg->event = event;
        int32_t ret;
        ret = sys_msg_put(ui_event_msg_id, (uint32_t)p_msg, 0);
        if(ret != RETVAL(E_OK))
        {
            mem_free(p_msg);
        }
    }
}

static void ui_event_init(void)
{
    sys_thread_create(SYS_THREAD(ui_event_task), NULL);
    ui_event_msg_id = sys_msg_create(SYS_MSG_Q(ui_event_msg), NULL);

    key_event_register_callback(KEY0_POWER, ui_event_handle);
    key_init(KEY0_POWER, &ui_event_cfg[KEY0_POWER]);

    key_event_register_callback(KEY1_HALL, ui_event_handle);
    key_init(KEY1_HALL, &ui_event_cfg[KEY1_HALL]);

    key_event_register_callback(KEY2_USB, ui_event_handle);
    key_init(KEY2_USB, &ui_event_cfg[KEY2_USB]);
}

APP_INITCALL("ui_event_init", ui_event_init);
