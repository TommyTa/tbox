/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file test_bsp_spi.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "modules.h"
#include "atcmd.h"
#include "errorno.h"
#include "x_stype.h"

#include "bsp_spi.h"

#define BASE_DEC_NUM_ERR                "\r\nEnter a decimal number\r\n"
#define BASE_HEX_NUM_ERR                "\r\nEnter a hexadecimal number\r\n"

static int32_t atcmd_spi_transfer(char* argv[], int argc)
{
    int16_t  tx_len;
    int16_t  addr_len;
    bsp_spi_bus_t spi_bus;
    uint8_t tx_buffer[40] = {0};
    uint8_t rx_buffer[40] = {0};
    char* temp;
    int16_t i = 0;
    char at_buf[40] = {0};
    char buf[8];

    if (argc < 3)
    {
        atcmd_send_response(AT_ERROR, AT_STR_ERR_ARGC, AT_STR_LEN(AT_STR_ERR_ARGC));
        return RETVAL(E_NULL);
    }

    if (is_str_digit(argv[0]) && is_str_digit(argv[1]) && is_str_digit(argv[2]))
    {
        spi_bus = (bsp_spi_bus_t)atoi(argv[0]);
        addr_len = (int16_t)atoi(argv[1]);
        tx_len = (int16_t)atoi(argv[2]);
    }
    else
    {
        atcmd_send_response(AT_ERROR, BASE_DEC_NUM_ERR, AT_STR_LEN(BASE_DEC_NUM_ERR));
        return RETVAL(E_FAIL);
    }

    for (i = 0; i < tx_len; i++)
    {
        if (is_str_hex(argv[i+3]))
        {
            tx_buffer[i] = (uint8_t)strtol(argv[i + 3], &temp, 16);
        }
        else
        {
            atcmd_send_response(AT_ERROR, BASE_HEX_NUM_ERR, AT_STR_LEN(BASE_HEX_NUM_ERR));
            return RETVAL(E_FAIL);
        }
          
    }

    bsp_spi_transfer(spi_bus, tx_buffer, tx_len, rx_buffer);            
    for (i = addr_len; i < tx_len; )
    {
        itoa((int)rx_buffer[i], buf, 10);
        strcat(at_buf, buf);
        i++;
        if (i < tx_len)
        {
            strcat(at_buf, ",");
        }
    }

    atcmd_send_response(AT_OK, at_buf, strlen(at_buf));
    return RETVAL(E_OK);
}

ATCMD_INIT("AT^TSPIRW=", 0, atcmd_spi_transfer);
