/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file display.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys_cmsis.h"
#include "modules.h"
#include "errorno.h"
#include "log.h"
#include "led_dev.h"
#include "atcmd.h"

#include "ui_indicator.h"

const led_entry_t indicator_entrys_pairing[] =
{
    {
        .color      = LED_COLOR_RED,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .ton        = 200,
        .trise      = 100,
        .tfall      = 100,
        .toff       = 0,
        .bright     = 255,
    },
    {
        .color      = LED_COLOR_YELLOW,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .ton        = 200,
        .trise      = 100,
        .tfall      = 100,
        .toff       = 0,
        .bright     = 255,
    },
};

const led_entry_t indicator_entrys_off_led[] =
{
    {
        .color      = LED_COLOR_BLACK,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .trise      = 0,
        .ton        = 0,
        .tfall      = 0,
        .toff       = 0,
        .bright     = 0,
    },
};

const led_entry_t indicator_entrys_electricity_low[] =
{
    {
        .color      = LED_COLOR_RED,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .trise      = 100,
        .ton        = 10000,
        .tfall      = 0,
        .toff       = 0,
        .bright     = 255,
    },
};

const led_entry_t indicator_entrys_electricity_middle[] =
{
    {
        .color      = LED_COLOR_YELLOW,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .trise      = 100,
        .ton        = 10000,
        .tfall      = 0,
        .toff       = 0,
        .bright     = 255,
    },
};

const led_entry_t indicator_entrys_electricity_high[] =
{
    {
        .color      = LED_COLOR_GREEN,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .trise      = 100,
        .ton        = 10000,
        .tfall      = 0,
        .toff       = 0,
        .bright     = 255,
    },
};

const led_entry_t indicator_entrys_charging_low[] =
{
    {
        .color      = LED_COLOR_RED,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .trise      = 100,
        .ton        = 2000,
        .tfall      = 0,
        .toff       = 0,
        .bright     = 255,
    },
};

const led_entry_t indicator_entrys_charging_middle[] =
{
    {
        .color      = LED_COLOR_YELLOW,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .trise      = 100,
        .ton        = 2000,
        .tfall      = 0,
        .toff       = 0,
        .bright     = 255,
    },
};

const led_entry_t indicator_entrys_charging_high[] =
{
    {
        .color      = LED_COLOR_GREEN,
        .mode       = LED_MODE_STEADY,
        .delay      = 0,
        .trise      = 100,
        .ton        = 2000,
        .tfall      = 0,
        .toff       = 0,
        .bright     = 255,
    },
};

const led_entry_t indicator_entrys_ear_pairing[] =
{
    {
        .color      = LED_COLOR_WHITE,
        .mode       = LED_MODE_BREATH,
        .delay      = 0,
        .trise      = 1000,
        .ton        = 2000,
        .tfall      = 1000,
        .toff       = 1000,
        .bright     = 255,
    },
};

const led_entry_t indicator_entrys_ear_reset[] =
{
    {
        .color      = LED_COLOR_RED,
        .mode       = LED_MODE_BREATH,
        .delay      = 0,
        .trise      = 1000,
        .ton        = 2000,
        .tfall      = 1000,
        .toff       = 1000,
        .bright     = 255,
    },
};

const led_pattern_t indicator_patterns[IND_ID_MAX] =
{
    /* IND_ID_OFF_LED */
    {
        .entrys     = indicator_entrys_off_led,
        .entrys_num = ARRAY_SIZE(indicator_entrys_off_led),
        .repeat_num = 0,
    },
    /* IND_ID_ELECTRICITY_LOW */
    {
        .entrys     = indicator_entrys_electricity_low,
        .entrys_num = ARRAY_SIZE(indicator_entrys_electricity_low),
        .repeat_num = 1,
    },
    /* IND_ID_ELECTRICITY_MIDDLE */
    {
        .entrys     = indicator_entrys_electricity_middle,
        .entrys_num = ARRAY_SIZE(indicator_entrys_electricity_middle),
        .repeat_num = 1,
    },
    /* IND_ID_ELECTRICITY_HIGH */
    {
        .entrys     = indicator_entrys_electricity_high,
        .entrys_num = ARRAY_SIZE(indicator_entrys_electricity_high),
        .repeat_num = 1,
    },
    /* IND_ID_CHARGING_LOW */
    {
        .entrys     = indicator_entrys_charging_low,
        .entrys_num = ARRAY_SIZE(indicator_entrys_charging_low),
        .repeat_num = 0,
    },
    /* IND_ID_CHARGING_MIDDLE */
    {
        .entrys     = indicator_entrys_charging_middle,
        .entrys_num = ARRAY_SIZE(indicator_entrys_charging_middle),
        .repeat_num = 0,
    },
    /* IND_ID_CHARGING_HIGH */
    {
        .entrys     = indicator_entrys_charging_high,
        .entrys_num = ARRAY_SIZE(indicator_entrys_charging_high),
        .repeat_num = 0,
    },
    /* IND_ID_OFF_LED(EAR) */
    {
        .entrys     = indicator_entrys_off_led,
        .entrys_num = ARRAY_SIZE(indicator_entrys_off_led),
        .repeat_num = 0,
    },
    /* IND_ID_ELECTRICITY_LOW(EAR) */
    {
        .entrys     = indicator_entrys_electricity_low,
        .entrys_num = ARRAY_SIZE(indicator_entrys_electricity_low),
        .repeat_num = 1,
    },
    /* IND_ID_ELECTRICITY_MIDDLE(EAR) */
    {
        .entrys     = indicator_entrys_electricity_middle,
        .entrys_num = ARRAY_SIZE(indicator_entrys_electricity_middle),
        .repeat_num = 1,
    },
    /* IND_ID_ELECTRICITY_HIGH(EAR) */
    {
        .entrys     = indicator_entrys_electricity_high,
        .entrys_num = ARRAY_SIZE(indicator_entrys_electricity_high),
        .repeat_num = 1,
    },
    /* IND_ID_CHARGING_LOW(EAR) */
    {
        .entrys     = indicator_entrys_charging_low,
        .entrys_num = ARRAY_SIZE(indicator_entrys_charging_low),
        .repeat_num = 0,
    },
    /* IND_ID_CHARGING_MIDDLE(EAR) */
    {
        .entrys     = indicator_entrys_charging_middle,
        .entrys_num = ARRAY_SIZE(indicator_entrys_charging_middle),
        .repeat_num = 0,
    },
    /* IND_ID_CHARGING_HIGH(EAR) */
    {
        .entrys     = indicator_entrys_charging_high,
        .entrys_num = ARRAY_SIZE(indicator_entrys_charging_high),
        .repeat_num = 0,
    },
    /* IND_ID_PAIRING(EAR) */
    {
        .entrys     = indicator_entrys_ear_pairing,
        .entrys_num = ARRAY_SIZE(indicator_entrys_ear_pairing),
        .repeat_num = 0,
    },
    /* IND_ID_RESET(EAR) */
    {
        .entrys     = indicator_entrys_ear_reset,
        .entrys_num = ARRAY_SIZE(indicator_entrys_ear_reset),
        .repeat_num = 5,
    },
};

const led_ind_t indicator_ctrls[IND_ID_MAX] =
{
    /* IND_ID_OFF_LED */
    {
        LED_CHAN_0,
        &indicator_patterns[IND_ID_OFF_LED],
    },
    /* IND_ID_ELECTRICITY_LOW */
    {
        LED_CHAN_0,
        &indicator_patterns[IND_ID_ELECTRICITY_LOW],
    },
    /* IND_ID_ELECTRICITY_MIDDLE */
    {
        LED_CHAN_0,
        &indicator_patterns[IND_ID_ELECTRICITY_MIDDLE],
    },
    /* IND_ID_ELECTRICITY_HIHG */
    {
        LED_CHAN_0,
        &indicator_patterns[IND_ID_ELECTRICITY_HIHG],
    },
    /* IND_ID_CHARGING_LOW */
    {
        LED_CHAN_0,
        &indicator_patterns[IND_ID_CHARGING_LOW],
    },
    /* IND_ID_CHARGING_MIDDLE */
    {
        LED_CHAN_0,
        &indicator_patterns[IND_ID_CHARGING_MIDDLE],
    },
    /* IND_ID_CHARGING_HIHG */
    {
        LED_CHAN_0,
        &indicator_patterns[IND_ID_CHARGING_HIHG],
    },
    /* IND_ID_OFF_LED(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_EAR_OFF_LED],
    },
    /* IND_ID_ELECTRICITY_LOW(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_EAR_ELECTRICITY_LOW],
    },
    /* IND_ID_ELECTRICITY_MIDDLE(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_EAR_ELECTRICITY_MIDDLE],
    },
    /* IND_ID_ELECTRICITY_HIHG(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_EAR_ELECTRICITY_HIHG],
    },
    /* IND_ID_CHARGING_LOW(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_EAR_CHARGING_LOW],
    },
    /* IND_ID_CHARGING_MIDDLE(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_EAR_CHARGING_MIDDLE],
    },
    /* IND_ID_CHARGING_HIHG(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_EAR_CHARGING_HIHG],
    },
    /* IND_ID_PAIRING(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_PAIRING],
    },
    /* IND_ID_RESET(EAR) */
    {
        LED_CHAN_1,
        &indicator_patterns[IND_ID_RESET],
    },
};

static device_t* led_dev = NULL;

int32_t ui_indicator(ind_id_t id)
{
    int32_t ret;

    if (led_dev == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (id >= IND_ID_MAX)
    {
        return RETVAL(E_FAIL);
    }

    ret = device_ioctl(led_dev, DEVICE_CTRL_CONFIG, (void *)&indicator_ctrls[id]);
    if (ret < 0)
    {
        LOG_E("[app_led][led_task]:device_ioctl white fail=%d\r\n", ret);
    }

    return RETVAL(E_OK);
}

static void ui_indicator_init(void)
{
    int32_t ret;

    led_dev = device_find("dev-led");
    if (led_dev == NULL)
    {
        LOG_E("[indicator][task init]:device_find fail\r\n");
        return;
    }

    ret = device_open(led_dev, DEVICE_OFLAG_RDWR);
    if (ret != RETVAL(E_OK))
    {
        LOG_E("[indicator][task init]:device_open fail\r\n");
        return;
    }
}

APP_INITCALL("ui_indicator", ui_indicator_init);

static int32_t ui_indicator_cmd(char* argv[], int argc)
{
    uint8_t id;

    if (argc != 1)
    {
        return RETVAL(E_INVAL_LEN);
    }

    id = (uint8_t)atoi(argv[0]);
    ui_indicator(id);

    return 0;
}

static int32_t ui_ledstate(char* argv[], int argc)
{
    led_dev_state_t led_state;
    char ret[5];

    if (argc != 1)
    {
        return RETVAL(E_INVAL_LEN);
    }

    led_state.chan = (uint8_t)atoi(argv[0]);

    device_ioctl(led_dev, DEVICE_CTRL_STATE, (void *)&led_state);
    itoa((int)led_state.state, ret, 10);
    atcmd_send_response(AT_OK, ret, strlen(ret));

    return 0;
}

ATCMD_INIT("AT^IND=", 0, ui_indicator_cmd);
ATCMD_INIT("AT^LEDSTATE=", 0, ui_ledstate);
