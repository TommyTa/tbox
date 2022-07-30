/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file test_bsp_flash.c
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
#include "bsp_flash.h"

#define MIN_LEN_OF_ATCMD    2
#define FLASH_PAGE_SIZE     2048

/**
 * @brief flash read at cmd
 * @param argv argc
 * @retval E_OK or E_FAIL
 */
static int32_t test_bsp_flash_read(char* argv[], int argc)
{
    int32_t ret = RETVAL(E_OK);
    uint8_t data[5];
    uint32_t addr;
    char* temp;
    char buf[8];
    char resp_buf[40];
    uint16_t len = 0;
    int i;

    if (argc != 2)
    {
        return RETVAL(E_INVAL_LEN);
    }

    if ((is_str_hex(argv[0]) == false) || (is_str_digit(argv[1]) == false))
    {
        return RETVAL(E_INVAL_PARM);
    }

    ret = RETVAL(E_OK);

    addr = (uint32_t)strtol(argv[0], &temp, 16);
    len = (uint16_t)atoi(argv[1]);

    memset(buf, 0 ,sizeof(buf));
    memset(resp_buf, 0 ,sizeof(resp_buf));

    bsp_flash_read_bytes(addr, data, len);

    for (i = 0; i < len; i++)
    {
        itoa((int)data[i], buf, 10);
        strcat(resp_buf, buf);
        if (i < (len - 1))
        {
            strcat(resp_buf, ",");
        }
    }
    atcmd_send_response(AT_OK, resp_buf, strlen(resp_buf));

    return ret;
}

/**
 * @brief flash write at cmd
 * @param argv argc
 * @retval RETVAL
 */
static int32_t test_bsp_flash_write(char* argv[], int argc)
{
    int32_t ret = E_OK;
    char* temp;
    uint8_t data[5];
    uint32_t addr;
    uint16_t len = 0;
    int i;

    if (argc < 6)
    {
        return RETVAL(E_INVAL_LEN);
    }

    if ((is_str_hex(argv[0]) == false) || (is_str_digit(argv[1]) == false))
    {
        return RETVAL(E_INVAL_PARM);
    }

    addr = (uint32_t)strtol(argv[0], &temp, 16);
    len = (uint16_t)atoi(argv[1]);

    if (len % sizeof(uint32_t) != 0)
    {
        return RETVAL(E_INVAL_LEN);
    }

    for (i = 0; i < len; i++)
    {
        data[i] = (uint8_t)strtol(argv[i + MIN_LEN_OF_ATCMD], &temp, 16);
    }

    ret = bsp_flash_write_words(addr, (uint32_t *)data, len);
    if (ret == E_OK)
    {
        atcmd_send_response(AT_OK, AT_STR_NONE, AT_STR_LEN(AT_STR_NONE));
    }

    return ret;
}

/**
 * @brief flash erase at cmd
 * @param argv argc
 * @retval E_OK or E_FAIL
 */
static int32_t test_bsp_flash_erase(char* argv[], int argc)
{
    int32_t ret = RETVAL(E_OK);
    uint32_t addr;
    char* temp;
    char buf[8];
    char resp_buf[40];

    if (argc != 1)
    {
        return RETVAL(E_INVAL_LEN);
    }

    if ((is_str_hex(argv[0]) == false))
    {
        return RETVAL(E_INVAL_PARM);
    }

    ret = RETVAL(E_OK);

    addr = (uint32_t)strtol(argv[0], &temp, 16);
    if (addr % FLASH_PAGE_SIZE != 0)
    {
        return RETVAL(E_INVAL_ADDR);
    }

    memset(buf, 0 ,sizeof(buf));
    memset(resp_buf, 0 ,sizeof(resp_buf));

    bsp_flash_erase_page(addr);

    atcmd_send_response(AT_OK, resp_buf, strlen(resp_buf));

    return ret;
}

ATCMD_INIT("AT^FLASHRD=", 0, test_bsp_flash_read);
ATCMD_INIT("AT^FLASHWR=", 0, test_bsp_flash_write);
ATCMD_INIT("AT^FLASHER=", 0, test_bsp_flash_erase);
