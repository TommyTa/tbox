/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file test_rsa.c
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

#include "rsa2048.h"

static unsigned char data[256] = {0};

static void hex_to_str(uint8_t *source, uint32_t len, uint8_t *target)
{
    uint8_t ddl, ddh;
    uint32_t i;

    for (i = 0; i < len; i++)
    {
        ddh = ('0' + source[i] / 16);
        ddl = ('0' + source[i] % 16);

        if (ddh > '9')
            ddh = (ddh + ('a' - '9' - 1));
        if (ddl > '9')
            ddl = (ddl + ('a' - '9' - 1));

        target[i * 2] = ddh;
        target[i * 2 + 1] = ddl;
    }
    target[len] = '\0';
}

/**
 * @brief rsa fill: fill data
 * @param argv argc
 * @retval E_OK or E_FAIL
 */
static int32_t test_rsa_sign(char *argv[], int argc)
{
    int32_t ret = RETVAL(E_OK);
    uint8_t id, i = 0;
    char *temp;
    char *p;
    uint8_t resp_buf[128];
    uint8_t buff[32];
    uint8_t len;

    if (argc != 2)
    {
        atcmd_send_response("\r\nERROR\r\n", "INVAL_LEN", strlen("INVAL_LEN"));
        return RETVAL(E_INVAL_LEN);
    }

    if ((is_str_digit(argv[0]) == false))
    {
        resp_buf[0] = *argv[0];
        atcmd_send_response(AT_ERROR, (const char *)resp_buf, 1);
        return RETVAL(E_INVAL_PARM);
    }

    i = 0;
    p = strtok(argv[1], "|");
    while (p != NULL)
    {
        buff[i++] = (uint8_t)strtol(p, &temp, 16);
        p = strtok(NULL, "|");
    }

    id = (uint8_t)atoi(argv[0]);
    len = sizeof(buff) / sizeof(uint8_t);
    for (i = 0; i < len; i++)
    {
        data[(id * len) + i] = buff[i];
    }

    hex_to_str(&data[(id * len)], len * 2, resp_buf);
    atcmd_send_response(AT_OK, (const char *)resp_buf, strlen((const char *)resp_buf));

    return ret;
}

/**
 * @brief RAS test : sign->md5
 * @param argv argc
 * @retval E_OK or E_FAIL
 */
static int32_t test_rsa_decrypt(char *argv[], int argc)
{
    int32_t ret = RETVAL(E_OK);
    uint8_t resp_buf[64];
    uint8_t decrypt[16];
    uint32_t out_len;

    ret = rsa_decrypt((uint8_t *)data, (sizeof(data) / sizeof(uint8_t)),
                      decrypt, (unsigned int *)&out_len);

    hex_to_str(decrypt, (sizeof(decrypt) / sizeof(uint8_t)) * 2, resp_buf);
    atcmd_send_response(AT_OK, (const char *)resp_buf, strlen((const char *)resp_buf));

    return ret;
}

ATCMD_INIT("AT^RSASIGN=", 0, test_rsa_sign);
ATCMD_INIT("AT^RSADECODE?", 0, test_rsa_decrypt);
