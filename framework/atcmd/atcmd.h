/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file atcmd.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _ATCMDH_
#define _ATCMDH_

#include <stdint.h>

#define AT_OK               "\r\nOK\r\n"
#define AT_ERROR            "\r\nERROR\r\n"

#define AT_STR_LEN(str)     strlen(str)
#define AT_STR_LEN_ZERO     0

#define AT_STR_NONE         ""
#define AT_STR_ERR_ARGC     "\r\nwrong number of arguments\r\n"
#define AT_STR_NO_CMD       "\r\ncommand not found\r\n"

void atcmd_send_response(char* result, const char* str, uint16_t len);

#endif // _ATCMDH_
