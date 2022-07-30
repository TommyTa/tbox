/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file test_trace.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "modules.h"
#include "atcmd.h"
#include "errorno.h"
#include "trace_area.h"
#include "trace.h"
#include "log.h"

static int32_t test_trace_write(char* argv[], int argc)
{
    if (argc != 1)
    {
        return RETVAL(E_INVAL_LEN);
    }

    switch (argv[0][0])
    {
        case '0':
        {
            LOG_D("AT^TTRACE=0");
        }
        break;

        case '1':
        {
            LOG_I("AT^TTRACE=1,%d", 128);
        }
        break;

        case '2':
        {
            LOG_N("AT^TTRACE=2,%d,%c", 65535, '2');
        }
        break;

        case '3':
        {
            LOG_W("AT^TTRACE=3,%d,%c,%xhuang", 0, '3', 0xA5);
        }
        break;

        case '4':
        {
            LOG_E("AT^TTRACE=4,%d,%c,%x,%u", -56, '4', 0xA5, 766);
        }
        break;

        case '5':
        {
            LOG_C("AT^TTRACE=5,%ld,%c,%lx", 65535, '5', 0x12AbCDA5);
        }
        break;

        default:
        break;
    }
    atcmd_send_response(AT_OK, AT_STR_NONE, AT_STR_LEN(AT_STR_NONE));

    return RETVAL(E_OK);
}

ATCMD_INIT("AT^WRLOG=", 0, test_trace_write);
