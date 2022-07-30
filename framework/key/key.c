/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ****************************************************************************
 * @file io_dev.c
 * @author tanxt
 * @version v1.0.0
 * ***************************************************************************/

#include "bsp_gpio.h"

#include <stddef.h>
#include "sys_cmsis.h"
#include "modules.h"
#include "errorno.h"
#include "string.h"
#include "log.h"

#include "key.h"

#define KEY_DEBUG_LOG_ENABLEx

#define CFG_KEY_LPRESS_THRESH_MS        2000
#define CFG_KEY_LLPRESS_THRESH_MS       8000
#define CFG_KEY_REPEAT_THRESH_MS        1000
#define CFG_KEY_DBLCLICK_THRESH_MS      200
#define CFG_KEY_TPLCLICK_THRESH_MS      180

typedef enum
{
    KEY_STATUS_NONE             = 0,
    KEY_STATUS_DEBOUNCE         = (1 << 0),
    KEY_STATUS_FIRST_PRESS      = (1 << 1),
    KEY_STATUS_FIRST_RELEASE    = (1 << 2),
    KEY_STATUS_SECOND_PRESS     = (1 << 3),
    KEY_STATUS_SECOND_RELEASE   = (1 << 4),
    KEY_STATUS_THIRD_PRESS      = (1 << 5),
    KEY_STATUS_THIRD_RELEASE    = (1 << 6),
} key_status_t;

typedef struct
{
    uint8_t status;
    uint32_t time;
    uint32_t event;
    uint32_t flag;
    uint32_t long_press_time;
    uint32_t long_long_press_time;
} key_data_t;

key_event_cb_t key_event_cb[KEY_NUM];

key_cfg_t key_cfg[KEY_NUM] = {0};
key_data_t key_data[KEY_NUM] = {0};

const uint8_t key_gpio_table[KEY_NUM] = {BSP_GPIO_0, BSP_GPIO_1, BSP_GPIO_2};

int32_t key_event_register_callback(key_num_t key, key_event_cb_t cb)
{
    if (cb == NULL)
    {
        return RETVAL(E_NULL);
    }

    key_event_cb[key] = cb;

    return RETVAL(E_OK);
}

static void key0_irq_handler(void)
{
    if (key_cfg[KEY0_POWER].trigger == KEY_TRIGGER_MODE_LEVEL)
    {
        if (key_data[KEY0_POWER].time < key_cfg[KEY0_POWER].effective_interval / KEY_SAMPLE_INTERVAL_MS)
        {
            return;
        }
        if (key_cfg[KEY0_POWER].active_level == bsp_gpio_read(key_gpio_table[KEY0_POWER]))
        {
            if (key_data[KEY0_POWER].status == KEY_STATUS_NONE)
            {
                key_data[KEY0_POWER].status               = KEY_STATUS_DEBOUNCE;
                key_data[KEY0_POWER].time                 = 0;
                key_data[KEY0_POWER].long_press_time      = 0;
                key_data[KEY0_POWER].long_long_press_time = 0;
            }
        }
    }
    else if (key_cfg[KEY0_POWER].trigger == KEY_TRIGGER_MODE_PULSE)
    {
        if (key_data[KEY0_POWER].time < key_cfg[KEY0_POWER].effective_interval / KEY_SAMPLE_INTERVAL_MS)
        {
            return;
        }
        if (key_cfg[KEY0_POWER].active_level == bsp_gpio_read(key_gpio_table[KEY0_POWER]))
        {
            key_data[KEY0_POWER].event |= KEY_EVENT_PRESS;
            if (key_event_cb[KEY0_POWER] != NULL)
            {
                (*key_event_cb[KEY0_POWER])(KEY0_POWER, KEY_EVENT_PRESS);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][press]\r\n", KEY1_HALL);
#endif

        }
        else if (key_cfg[KEY0_POWER].active_level != bsp_gpio_read(key_gpio_table[KEY0_POWER]))
        {
            key_data[KEY0_POWER].time = 0;
            key_data[KEY0_POWER].event |= KEY_EVENT_RELEASE;
            if (key_event_cb[KEY0_POWER] != NULL)
            {
                (*key_event_cb[KEY0_POWER])(KEY0_POWER, KEY_EVENT_RELEASE);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][release]\r\n", KEY0_POWER);
#endif

        }
    }
}

static void key1_irq_handler(void)
{
    if (key_cfg[KEY1_HALL].trigger == KEY_TRIGGER_MODE_LEVEL)
    {
        if (key_data[KEY1_HALL].time < key_cfg[KEY1_HALL].effective_interval / KEY_SAMPLE_INTERVAL_MS)
        {
            return;
        }
        if (key_cfg[KEY1_HALL].active_level == bsp_gpio_read(key_gpio_table[KEY1_HALL]))
        {
            if (key_data[KEY1_HALL].status == KEY_STATUS_NONE)
            {
                key_data[KEY1_HALL].status               = KEY_STATUS_DEBOUNCE;
                key_data[KEY1_HALL].time                 = 0;
                key_data[KEY1_HALL].long_press_time      = 0;
                key_data[KEY1_HALL].long_long_press_time = 0;
            }
        }
    }
    else if (key_cfg[KEY1_HALL].trigger == KEY_TRIGGER_MODE_PULSE)
    {
        if (key_data[KEY1_HALL].time < key_cfg[KEY1_HALL].effective_interval / KEY_SAMPLE_INTERVAL_MS)
        {
            return;
        }
        if (key_cfg[KEY1_HALL].active_level == bsp_gpio_read(key_gpio_table[KEY1_HALL]))
        {
            key_data[KEY1_HALL].event |= KEY_EVENT_PRESS;
            if (key_event_cb[KEY1_HALL] != NULL)
            {
                (*key_event_cb[KEY1_HALL])(KEY1_HALL, KEY_EVENT_PRESS);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][press]\r\n", KEY1_HALL);
#endif

        }
        else if (key_cfg[KEY1_HALL].active_level != bsp_gpio_read(key_gpio_table[KEY1_HALL]))
        {
            key_data[KEY1_HALL].time = 0;
            key_data[KEY1_HALL].event |= KEY_EVENT_RELEASE;
            if (key_event_cb[KEY1_HALL] != NULL)
            {
                (*key_event_cb[KEY1_HALL])(KEY1_HALL, KEY_EVENT_RELEASE);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][release]\r\n", KEY1_HALL);
#endif

        }
    }
}

static void key2_irq_handler(void)
{
    if (key_cfg[KEY2_USB].trigger == KEY_TRIGGER_MODE_LEVEL)
    {
        if (key_data[KEY2_USB].time < key_cfg[KEY2_USB].effective_interval / KEY_SAMPLE_INTERVAL_MS)
        {
            return;
        }
        if (key_cfg[KEY2_USB].active_level == bsp_gpio_read(key_gpio_table[KEY2_USB]))
        {
            if (key_data[KEY2_USB].status == KEY_STATUS_NONE)
            {
                key_data[KEY2_USB].status               = KEY_STATUS_DEBOUNCE;
                key_data[KEY2_USB].time                 = 0;
                key_data[KEY2_USB].long_press_time      = 0;
                key_data[KEY2_USB].long_long_press_time = 0;
            }
        }
    }
    else if (key_cfg[KEY2_USB].trigger == KEY_TRIGGER_MODE_PULSE)
    {
        if (key_data[KEY2_USB].time < key_cfg[KEY2_USB].effective_interval / KEY_SAMPLE_INTERVAL_MS)
        {
            return;
        }
        if (key_cfg[KEY2_USB].active_level == bsp_gpio_read(key_gpio_table[KEY2_USB]))
        {
            key_data[KEY2_USB].event |= KEY_EVENT_PRESS;
            if (key_event_cb[KEY2_USB] != NULL)
            {
                (*key_event_cb[KEY2_USB])(KEY2_USB, KEY_EVENT_PRESS);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][press]\r\n", KEY2_USB);
#endif

        }
        else if (key_cfg[KEY2_USB].active_level != bsp_gpio_read(key_gpio_table[KEY2_USB]))
        {
            key_data[KEY2_USB].time = 0;
            key_data[KEY2_USB].event |= KEY_EVENT_RELEASE;
            if (key_event_cb[KEY2_USB] != NULL)
            {
                (*key_event_cb[KEY2_USB])(KEY2_USB, KEY_EVENT_RELEASE);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][release]\r\n", KEY2_USB);
#endif

        }
    }
}

void bsp_key_para_cfg(key_num_t key, key_cfg_t *cfg)
{
    key_cfg[key].active_level               = cfg->active_level;
    key_cfg[key].debounce_time              = cfg->debounce_time;
    key_cfg[key].effective_interval         = cfg->effective_interval;
    key_cfg[key].trigger                    = cfg->trigger;
}

void key_init(key_num_t key, key_cfg_t *cfg)
{
    memset(&key_data[key], 0, sizeof(key_data_t));
    bsp_key_para_cfg(key, cfg);

    if (key == KEY0_POWER)
    {
        bsp_gpio_register_irq_callback(BSP_GPIO_0, key0_irq_handler);
    }
    else if (key == KEY1_HALL)
    {
        bsp_gpio_register_irq_callback(BSP_GPIO_1, key1_irq_handler);
    }
    else if (key == KEY2_USB)
    {
        bsp_gpio_register_irq_callback(BSP_GPIO_2, key2_irq_handler);
    }

    bsp_gpio_init(key_gpio_table[key], BSP_GPIO_EXTI_ENABLE);
}

sys_timer_id_t key_sample_timer_id;

static void key_sample_timer_normal_handle(key_num_t key)
{
    if (key_cfg[key].active_level == bsp_gpio_read(key_gpio_table[key]))
    {
        if (key_data[key].status == KEY_STATUS_NONE)
        {
            key_data[key].status               = KEY_STATUS_DEBOUNCE;
            key_data[key].time                 = 0;
            key_data[key].long_press_time      = 0;
            key_data[key].long_long_press_time = 0;

#ifdef KEY_DEBUG_LOG_ENABLE
            LOG_D("[key][%d][normal_handle]\r\n", key);
#endif
        }
    }
}

static void key_sample_timer_debounce_handle(key_num_t key)
{
    if (key_data[key].time >= key_cfg[key].debounce_time / KEY_SAMPLE_INTERVAL_MS)
    {
        key_data[key].time                  = 0;
        key_data[key].long_press_time       = 0;
        key_data[key].long_long_press_time  = 0;
        key_data[key].flag                  = 0;
        key_data[key].event                 = 0;
        if (key_cfg[key].active_level == bsp_gpio_read(key_gpio_table[key]))
        {
            key_data[key].event |= KEY_EVENT_PRESS;
            if (key_event_cb[key] != NULL)
            {
                (*key_event_cb[key])(key, KEY_EVENT_PRESS);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
            LOG_D("[key][%d][press]\r\n", key);
#endif

            key_data[key].flag |= KEY_EVENT_PRESS;
            key_data[key].status = KEY_STATUS_FIRST_PRESS;
        }
        else
        {
            key_data[key].status = KEY_STATUS_NONE;
        }
    }
}

static void key_sample_timer_first_press_handle(key_num_t key)
{
    if (key_cfg[key].active_level == bsp_gpio_read(key_gpio_table[key]))
    {
        if (key_data[key].long_press_time >= CFG_KEY_LPRESS_THRESH_MS / KEY_SAMPLE_INTERVAL_MS)
        {
            key_data[key].long_press_time = 0;

            if ((key_data[key].event & KEY_EVENT_LONG_HOLD) == 0)
            {
                key_data[key].event |= KEY_EVENT_LONG_HOLD;
                key_data[key].flag |= KEY_EVENT_LONG_HOLD;
                if (key_event_cb[key] != NULL)
                {
                    (*key_event_cb[key])(key, KEY_EVENT_LONG_HOLD);
                }

#ifdef KEY_DEBUG_LOG_ENABLE
                LOG_D("[key][%d][long_hold]\r\n", key);
#endif

            }
        }

        if (key_data[key].long_long_press_time >= CFG_KEY_LLPRESS_THRESH_MS / KEY_SAMPLE_INTERVAL_MS)
        {
            key_data[key].long_long_press_time = 0;

            if ((key_data[key].event & KEY_EVENT_LONG_LONG_HOLD) == 0)
            {
                key_data[key].event |= KEY_EVENT_LONG_LONG_HOLD;
                key_data[key].flag |= KEY_EVENT_LONG_LONG_HOLD;
                if (key_event_cb[key] != NULL)
                {
                    (*key_event_cb[key])(key, KEY_EVENT_LONG_LONG_HOLD);
                }

#ifdef KEY_DEBUG_LOG_ENABLE
                LOG_D("[key][%d][long_long_hold]\r\n", key);
#endif

            }
        }
    }
    else
    {
        key_data[key].event |= KEY_EVENT_RELEASE;
        if (key_event_cb[key] != NULL)
        {
            (*key_event_cb[key])(key, KEY_EVENT_RELEASE);
        }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][release]\r\n", key);
#endif

        if (key_data[key].flag & KEY_EVENT_LONG_LONG_HOLD)
        {
            key_data[key].flag &= (~KEY_EVENT_LONG_LONG_HOLD);
            key_data[key].event |= KEY_EVENT_LONG_LONG_RELEASE;
            if (key_event_cb[key] != NULL)
            {
                (*key_event_cb[key])(key, KEY_EVENT_LONG_LONG_RELEASE);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
            LOG_D("[key][%d][long_long_release]\r\n", key);
#endif

        }
        else
        {
            if (key_data[key].flag & KEY_EVENT_LONG_HOLD)
            {
                key_data[key].flag &= (~KEY_EVENT_LONG_HOLD);
                key_data[key].event |= KEY_EVENT_LONG_RELEASE;
                if (key_event_cb[key] != NULL)
                {
                    (*key_event_cb[key])(key, KEY_EVENT_LONG_RELEASE);
                }

#ifdef KEY_DEBUG_LOG_ENABLE
                LOG_D("[key][%d][long_release]\r\n", key);
#endif

            }
        }

        if (key_data[key].time >= CFG_KEY_DBLCLICK_THRESH_MS / KEY_SAMPLE_INTERVAL_MS)
        {
            if (((key_data[key].event & KEY_EVENT_LONG_LONG_HOLD) == 0) && ((key_data[key].event & KEY_EVENT_LONG_HOLD) == 0))
            {
                key_data[key].event |= KEY_EVENT_CLICK;
                if (key_event_cb[key] != NULL)
                {
                    (*key_event_cb[key])(key, KEY_EVENT_CLICK);
                }

#ifdef KEY_DEBUG_LOG_ENABLE
                LOG_D("[key][%d][click_1]\r\n", key);
#endif

            }

            key_data[key].status = KEY_STATUS_NONE;
        }
        else
        {
            key_data[key].time      = 0;
            key_data[key].status    = KEY_STATUS_FIRST_RELEASE;
        }
    }
}

static void key_sample_timer_first_release_handle(key_num_t key)
{
    if (key_data[key].time <= CFG_KEY_DBLCLICK_THRESH_MS / KEY_SAMPLE_INTERVAL_MS)
    {
        if (key_cfg[key].active_level == bsp_gpio_read(key_gpio_table[key]))
        {
            key_data[key].time      = 0;
            key_data[key].status    = KEY_STATUS_SECOND_PRESS;
        }
    }
    else
    {
        if (key_cfg[key].active_level != bsp_gpio_read(key_gpio_table[key]))
        {
            key_data[key].event |= KEY_EVENT_CLICK;
            if (key_event_cb[key] != NULL)
            {
                (*key_event_cb[key])(key, KEY_EVENT_CLICK);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
            LOG_D("[key][%d][click_2]\r\n", key);
#endif

            key_data[key].status = KEY_STATUS_NONE;
        }
    }
}

static void key_sample_timer_second_press_handle(key_num_t key)
{
    if (key_data[key].time <= CFG_KEY_DBLCLICK_THRESH_MS / KEY_SAMPLE_INTERVAL_MS)
    {
        if (key_cfg[key].active_level != bsp_gpio_read(key_gpio_table[key]))
        {
            key_data[key].time      = 0;
            key_data[key].status    = KEY_STATUS_SECOND_RELEASE;
        }
    }
    else
    {
        key_data[key].event |= KEY_EVENT_CLICK;
        if (key_event_cb[key] != NULL)
        {
            (*key_event_cb[key])(key, KEY_EVENT_CLICK);
        }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][click_3]\r\n", key);
#endif

        key_data[key].time                  = 0;
        key_data[key].long_press_time       = 0;
        key_data[key].long_long_press_time  = 0;
        key_data[key].status                = KEY_STATUS_FIRST_PRESS;
    }
}

static void key_sample_timer_second_release_handle(key_num_t key)
{
    if (key_data[key].time <= CFG_KEY_TPLCLICK_THRESH_MS / KEY_SAMPLE_INTERVAL_MS)
    {
        if (key_cfg[key].active_level == bsp_gpio_read(key_gpio_table[key]))
        {
            key_data[key].time      = 0;
            key_data[key].status    = KEY_STATUS_THIRD_PRESS;
        }
    }
    else
    {
        key_data[key].event |= KEY_EVENT_DOUBLE_CLICK;
        if (key_event_cb[key] != NULL)
        {
            (*key_event_cb[key])(key, KEY_EVENT_DOUBLE_CLICK);
        }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][double_click_1]\r\n", key);
#endif

        key_data[key].status = KEY_STATUS_NONE;
    }
}

static void key_sample_timer_third_press_handle(key_num_t key)
{
    if (key_data[key].time <= CFG_KEY_TPLCLICK_THRESH_MS / KEY_SAMPLE_INTERVAL_MS)
    {
        if (key_cfg[key].active_level != bsp_gpio_read(key_gpio_table[key]))
        {
            key_data[key].time = 0;
            key_data[key].event |= KEY_EVENT_TRIPLE_CLICK;
            if (key_event_cb[key] != NULL)
            {
                (*key_event_cb[key])(key, KEY_EVENT_TRIPLE_CLICK);
            }

#ifdef KEY_DEBUG_LOG_ENABLE
            LOG_D("[key][%d][triple_click]\r\n", key);
#endif

            key_data[key].status = KEY_STATUS_NONE;
        }
    }
    else
    {
        key_data[key].event |= KEY_EVENT_DOUBLE_CLICK;
        if (key_event_cb[key] != NULL)
        {
            (*key_event_cb[key])(key, KEY_EVENT_DOUBLE_CLICK);
        }

#ifdef KEY_DEBUG_LOG_ENABLE
        LOG_D("[key][%d][double_click_2]\r\n", key);
#endif

        key_data[key].time                  = 0;
        key_data[key].long_press_time       = 0;
        key_data[key].long_long_press_time  = 0;
        key_data[key].status                = KEY_STATUS_FIRST_PRESS;
    }
}

static void key_sample_timer_handle(void const *arg)
{
    uint16_t i;

    for (i = 0; i < KEY_NUM; i++)
    {
        key_data[i].time ++;
        key_data[i].long_press_time ++;
        key_data[i].long_long_press_time ++;
    }

    for (i = 0; i < KEY_NUM; i++)
    {
        if (key_cfg[i].trigger == KEY_TRIGGER_MODE_PULSE)
        {
            continue;
        }
        switch (key_data[i].status)
        {
            case KEY_STATUS_NONE:
            {
                key_sample_timer_normal_handle(i);
            }
            break;

            case KEY_STATUS_DEBOUNCE:
            {
                key_sample_timer_debounce_handle(i);
            }
            break;

            case KEY_STATUS_FIRST_PRESS:
            {
                key_sample_timer_first_press_handle(i);
            }
            break;

            case KEY_STATUS_FIRST_RELEASE:
            {
                key_sample_timer_first_release_handle(i);
            }
            break;

            case KEY_STATUS_SECOND_PRESS:
            {
                key_sample_timer_second_press_handle(i);
            }
            break;

            case KEY_STATUS_SECOND_RELEASE:
            {
                key_sample_timer_second_release_handle(i);
            }
            break;

            case KEY_STATUS_THIRD_PRESS:
            {
                key_sample_timer_third_press_handle(i);
            }
            break;

            case KEY_STATUS_THIRD_RELEASE:
            {
                ;
            }
            break;

            default:
            break;
        }
    }
}

SYS_TIMER_DEF(key_sample_timer, key_sample_timer_handle);

static void key_sample_timer_init(void)
{
    sys_status status;

    key_sample_timer_id = sys_timer_create(SYS_TIMER(key_sample_timer),
                        SYS_TIMER_PERIODIC, NULL);
    if (key_sample_timer_id)
    {
        status = sys_timer_start(key_sample_timer_id, KEY_SAMPLE_INTERVAL_MS);
        if (status != RETVAL(E_OK))
        {
            ;
        }
    }
}

APP_INITCALL("key_sample_timer", key_sample_timer_init);
