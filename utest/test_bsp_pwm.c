/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file test_bsp_pwm.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "modules.h"
#include "atcmd.h"
#include "bsp_pwm.h"
#include "errorno.h"
#include "log.h"

static int32_t test_bsp_pwm(char* argv[], int argc)
{
    uint8_t val;
    int pwm_tim = BSP_PWM_1;
    char* temp;

    val = (uint8_t)strtol(argv[1], &temp, 10);
    if (argc != 2)
    {
        atcmd_send_response(AT_ERROR, AT_STR_ERR_ARGC, AT_STR_LEN(AT_STR_ERR_ARGC));
        return RETVAL(E_INVAL_LEN);
    }
    if (strncmp(argv[0], "PWM1", strlen("PWM1")) == 0)
    {
        pwm_tim = BSP_PWM_1;
    }
    else if (strncmp(argv[0], "PWM2", strlen("PWM2")) == 0)
    {
        pwm_tim = BSP_PWM_2;
    }
    else if (strncmp(argv[0], "PWM3", strlen("PWM3")) == 0)
    {
        pwm_tim = BSP_PWM_3;
    }
    else if (strncmp(argv[0], "PWM4", strlen("PWM4")) == 0)
    {
        pwm_tim = BSP_PWM_4;
    }
    else if (strncmp(argv[0], "PWM5", strlen("PWM5")) == 0)
    {
        pwm_tim = BSP_PWM_5;
    }
    else if (strncmp(argv[0], "PWM6", strlen("PWM6")) == 0)
    {
        pwm_tim = BSP_PWM_6;
    }
    else if (strncmp(argv[0], "PWM7", strlen("PWM7")) == 0)
    {
        pwm_tim = BSP_PWM_7;
    }
    else if (strncmp(argv[0], "PWM8", strlen("PWM8")) == 0)
    {
        pwm_tim = BSP_PWM_8;
    }
    else if (strncmp(argv[0], "PWM9", strlen("PWM9")) == 0)
    {
        pwm_tim = BSP_PWM_9;
    }
    else if (strncmp(argv[0], "PWMA", strlen("PWMA")) == 0)
    {
        pwm_tim = BSP_PWM_10;
    }
    bsp_pwm_set(pwm_tim, val);

    atcmd_send_response(AT_OK, AT_STR_NONE, AT_STR_LEN(AT_STR_NONE));

    return RETVAL(E_OK);
}

ATCMD_INIT("AT^TPWM=", 0, test_bsp_pwm);
