/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file trace_dump.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _TRACE_DUMP_H_
#define _TRACE_DUMP_H_

#define TRACE_POST_DATA_LEN                     DTCMD_DATA_LEN
#define NVM_PAGE_SIZE                           2048

#define DUMP_STR_STARTED                        "\r\nDump is started now, please send \"DT^DUMPNEXT\"\r\n"
#define DUMP_STR_STOPPED                        "\r\nDump is stopped, please send \"DT^DUMPLOG\"\r\n"
#define DUMP_STR_FINISH                         "\r\nDump finish\r\n"
#define DUMP_STR_BUSY                           "\r\nDump system is busy\r\n"

typedef enum
{
    DUMP_STEP_STOP = 0,
    DUMP_STEP_POST_DATA,
} dump_step_t;

#endif //_TRACE_DUMP_H_
