/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file test_bsp_led.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdint.h>
#include <string.h>

#include "modules.h"
#include "atcmd.h"
#include "bsp_led.h"
#include "errorno.h"
#include "log.h"

static int32_t test_bsp_led(char* argv[], int argc)
{
    bsp_led_t led = BSP_LED_0;

    if (argc != 2)
    {
        atcmd_send_response(AT_ERROR, AT_STR_ERR_ARGC, AT_STR_LEN(AT_STR_ERR_ARGC));
        return RETVAL(E_INVAL_LEN);
    }

    if (strncmp(argv[0], "0", strlen("0")) == 0)
    {
        led = BSP_LED_0;
    }
    else if (strncmp(argv[0], "1", strlen("1")) == 0)
    {
        led = BSP_LED_1;
    }

    if (strncmp(argv[1], "0", strlen("0")) == 0)
    {
        bsp_led_off(led);
    }
    else if (strncmp(argv[1], "1", strlen("1")) == 0)
    {
        bsp_led_on(led);
    }

    LOG_I("[utest][bsp_led][atcmd]:led=%d", led);

    atcmd_send_response(AT_OK, AT_STR_NONE, AT_STR_LEN(AT_STR_NONE));

    return RETVAL(E_OK);
}

ATCMD_INIT("AT^TLED=", 0, test_bsp_led);
