/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file test_md5.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "typedefs.h"
#include "modules.h"
#include "atcmd.h"
#include "errorno.h"
#include "x_stype.h"
#include "md5.h"


static void hex_to_str(uint8_t *source, uint32_t len, uint8_t *target)
{
    uint8_t ddl, ddh;
    uint32_t i;

    for (i = 0; i < len; i++)
    {
        ddh = ('0' + source[i] / 16);
        ddl = ('0' + source[i] % 16);

        if (ddh > '9')
            ddh = (ddh + ('a' - 58));
        if (ddl > '9')
            ddl = (ddl + ('a' - 58));

        target[i * 2] = ddh;
        target[i * 2 + 1] = ddl;
    }
    target[len] = '\0';
}

/**
 * @brief md5 test 2 : flash data -> md5
 * @param argv argc
 * @retval E_OK or E_FAIL
 */
static int32_t test_md5_addr(char* argv[], int argc)
{
    int32_t ret = RETVAL(E_OK);
    uint32_t addr;
    char* temp;
    uint8_t resp_buf[64];
    uint32_t len = 0;
    uint8_t decrypt[16];

    if (argc != 2)
    {
        return RETVAL(E_INVAL_LEN);
    }

    if ((is_str_hex(argv[0]) == false) || (is_str_digit(argv[1]) == false))
    {
        return RETVAL(E_INVAL_PARM);
    }

    addr = (uint32_t)strtol(argv[0], &temp, 16);
    len = (uint32_t)atoi(argv[1]);

    md5_sign((unsigned char *)addr, len, decrypt);

    hex_to_str(decrypt, (sizeof(decrypt) / sizeof(uint8_t)) * 2, resp_buf);
    atcmd_send_response(AT_OK, (const char *)resp_buf, strlen((const char *)resp_buf));

    return ret;
}

/**
 * @brief md5 test 1 : str->md5
 * @param argv argc
 * @retval E_OK or E_FAIL
 */
static int32_t test_md5(char* argv[], int argc)
{
    int32_t ret = RETVAL(E_OK);
    uint8_t resp_buf[64];
    uint8_t decrypt[16];

    if (argc != 1)
    {
        return RETVAL(E_INVAL_LEN);
    }

    md5_sign((unsigned char *)argv[0], strlen(argv[0]), decrypt);
    
    hex_to_str(decrypt, (sizeof(decrypt) / sizeof(uint8_t)) * 2, resp_buf);
    atcmd_send_response(AT_OK, (const char *)resp_buf, strlen((const char *)resp_buf));

    return ret;
}

ATCMD_INIT("AT^MD5=", 0, test_md5);
ATCMD_INIT("AT^MD5ADDR=", 0, test_md5_addr);
