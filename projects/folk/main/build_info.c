/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file build_info.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <string.h>
#include <stdint.h>

#include "modules.h"
#include "atcmd.h"

#ifndef REVISION_INFO
#define REVISION_INFO           "0.1.0.0"
#endif

#ifndef REVISION_TYPE
#define REVISION_TYPE           "factory"
#endif

#define BUILD_INFO_LOCATION     __attribute__((section(".build_info.")))
#define TO_STR_A(s)             # s
#define TO_STR(s)               TO_STR_A(s)

#define TRACE_CRLF
#ifdef TRACE_CRLF
#define NEW_LINE_STR            "\r\n"
#else
#define NEW_LINE_STR            "\n"
#endif

const char BUILD_INFO_LOCATION sys_build_info[] =
    NEW_LINE_STR "^VERSION:INTS:" REVISION_INFO_S
    NEW_LINE_STR "^VERSION:INTS:N32G4FR"
    NEW_LINE_STR "^VERSION:INTH:" REVISION_TYPE_S
    NEW_LINE_STR "^VERSION:EXTU:Folk"
    NEW_LINE_STR;

static int32_t get_build_info(char* argv[], int argc)
{
    atcmd_send_response(AT_OK, sys_build_info, strlen(sys_build_info));
    return 0;
}

ATCMD_INIT("AT^VERSION?", 0, get_build_info);
