/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file test_bsp_i2c.c
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
#include "bsp_i2c.h"

#define MAX_LEN_OF_PARAM    5
#define MIN_LEN_OF_PARAM    1
#define MIN_LEN_OF_ATCMD    4

/**
 * @brief i2c read at cmd
 * @param argv argc
 * @retval E_OK or E_FAIL
 */
static int32_t test_bsp_i2c_read(char* argv[], int argc)
{
    i2c_transfer_t i2c;
    int32_t ret;
    uint8_t data[5];
    uint8_t reg;
    char* temp;
    char buf[8];
    char resp_buf[40];
    uint16_t len = 0;
    int i;

    if (argc != 4)
    {
        return RETVAL(E_INVAL_LEN);
    }

    if ((is_str_hex(argv[0]) == false) || (is_str_hex(argv[1]) == false) ||
        (is_str_digit(argv[2]) == false) || (is_str_digit(argv[3]) == false))
    {
        return RETVAL(E_INVAL_PARM);
    }

    i2c.slave_addr = (uint8_t)strtol(argv[0], &temp, 16);

    reg = (uint8_t)strtol(argv[1], &temp, 16);
    i2c.bus = (uint8_t)atoi(argv[2]);
    len = (uint16_t)atoi(argv[3]);
    if (len < MIN_LEN_OF_PARAM || len > MAX_LEN_OF_PARAM)
    {
        return RETVAL(E_INVAL_PARM);
    }

    memset(buf, 0 ,sizeof(buf));
    memset(resp_buf, 0 ,sizeof(resp_buf));

    ret = bsp_i2c_read(&i2c, reg, data, len);
    if (ret == E_OK)
    {
        for (i=0; i<len; i++)
        {
            itoa((int)data[i], buf, 10);
            strcat(resp_buf, buf);
            if (i < (len-1))
            {
                strcat(resp_buf, ",");
            }
        }
        atcmd_send_response(AT_OK, resp_buf, strlen(resp_buf));
    }

    return ret;
}

/**
 * @brief i2c write at cmd
 * @param argv argc
 * @retval RETVAL
 */
static int32_t test_bsp_i2c_write(char* argv[], int argc)
{
    i2c_transfer_t i2c;
    int32_t ret = E_OK;
    char* temp;
    uint8_t data[5];
    uint8_t reg;
    uint16_t len = 0;
    int i;

    if (argc <= MIN_LEN_OF_ATCMD)
    {
        return RETVAL(E_INVAL_LEN);
    }

    if ((is_str_hex(argv[0]) == false) || (is_str_hex(argv[1]) == false) ||
        (is_str_digit(argv[2]) == false) || (is_str_digit(argv[3]) == false))
    {
        return RETVAL(E_INVAL_PARM);
    }

    i2c.slave_addr = (uint8_t)strtol(argv[0], &temp, 16);
    reg = (uint8_t)strtol(argv[1], &temp, 16);
    i2c.bus = (uint8_t)atoi(argv[2]);
    len = (uint16_t)atoi(argv[3]);
    if (len < MIN_LEN_OF_PARAM || len > MAX_LEN_OF_PARAM)
    {
        return RETVAL(E_INVAL_PARM);
    }
    len = len > (argc - MIN_LEN_OF_ATCMD) ? argc - MIN_LEN_OF_ATCMD : len;

    for (i = 0; i < len; i++)
    {
        if (is_str_hex(argv[i + MIN_LEN_OF_ATCMD]) &&
            (strlen(argv[i + MIN_LEN_OF_ATCMD]) == 3 ||
            strlen(argv[i + MIN_LEN_OF_ATCMD]) == 4))
        {
            data[i] = (uint8_t)strtol(argv[i + MIN_LEN_OF_ATCMD], &temp, 16);
        }
        else
        {
            return RETVAL(E_INVAL_PARM);
        }
    }

    ret = bsp_i2c_write(&i2c, reg, data, len);
    if (ret == E_OK)
    {
        atcmd_send_response(AT_OK, AT_STR_NONE, AT_STR_LEN(AT_STR_NONE));
    }

    return ret;
}

ATCMD_INIT("AT^I2CRD=", 0, test_bsp_i2c_read);
ATCMD_INIT("AT^I2CWR=", 0, test_bsp_i2c_write);
