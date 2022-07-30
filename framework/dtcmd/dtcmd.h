/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file dtcmd.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _DTCMDH_
#define _DTCMDH_

#include <stdint.h>
#include <string.h>
#include "errorno.h"
#include "modules.h"

#define DT_OK                       "\r\nOK\r\n"
#define DT_ERROR                    "\r\nERROR\r\n"

#define DT_STR_LEN(str)             strlen(str)
#define DT_STR_LEN_ZERO             0

#define DT_STR_NONE                 ""
#define DT_STR_ERR_ARGC             "\r\nwrong number of arguments\r\n"
#define DT_STR_NO_CMD               "\r\ncommand not found\r\n"
#define DT_STR_SYS_DISABLE          "\r\nsystem is disable\r\n"
#define DT_STR_SYS_BUSY             "\r\nsystem is busy\r\n"

#define DTCMD_DATA_LEN              200
#define DTCMD_MAX_LEN               30

#define DTCMD_RESP_DATA             NULL

#define DTCMD_INIT(dtcmd, cb)      dtcmd_register(dtcmd, cb)

typedef void (*dtcmd_handle_cb_t)(void);

void dtcmd_send_response(char* result, const char* str, uint16_t len);
void dtcmd_send_result(char* result, const char* str, uint16_t len);
int32_t dtcmd_register(char *str, dtcmd_handle_cb_t cb);
#endif // _DTCMDH_
