/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file dfu.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdio.h>
#include <string.h>

#include "partition.h"
#include "crc16.h"
#include "errorno.h"
#include "sys_cmsis.h"
#include "bsp_flash.h"
#include "bsp_uart.h"

#include "dfu.h"

#define FLASH_PAGE_SIZE                         2048
#define DFU_PACK_HEAD_1ST_BYTE                  0xA5
#define DFU_PACK_OPCODE_DATA                    0x5A
#define DFU_PACK_OPCODE_ABORT                   0xB6
#define GET_PACK_ID(BUFF)                       (BUFF[2])
#define GET_DATA_LEN(BUFF)                      (BUFF[3])
#define GET_DATA_HEAD_ADDR(BUFF)                (BUFF[4])
#define DFU_CRC16_INIT_VAL                      0xFFFF
#define RECV_TIMEOUT                            0xFFFF
#define FLASH_DATA_INVAL                        0xFFFFFFFF
#define BUFF_SIZE                               256
#define PACK_DATA_LEN                           128
#define PACK_MAX_PAGE                           16
#define PACK_HEADER_LEN                         4
#define PACK_CRC_LEN                            2

#define ENTER_CRITICAL()                        sys_irq_disable()
#define EXIT_CRITICAL()                         sys_irq_enable()

#define ENTER_DFU_MODE                          "enter dfu mode...\r\n"

typedef enum
{
    UPGRADE_FREE = 0,
    UPGRADE_ING,
    UPGRADE_END
} dfu_up_stage_t;

typedef enum
{
    RX_DATA_ING = 0,
    RX_DATA_END
} dfu_rx_stage_t;

typedef struct
{
    dfu_up_stage_t  up_stage;
    dfu_rx_stage_t  data_rx_stage;
    uint8_t         pages_num;
    uint16_t        rx_pack_num;
} dfu_t;
static dfu_t g_dfu = {0};

static uint8_t  rx_buff[BUFF_SIZE] = {0};
static uint8_t  flash_buf[FLASH_PAGE_SIZE] = {0};
static uint16_t data_rx_timeout = 0;

void dfu_jump_app(void)
{
    if (bsp_flash_read_word(FLASH_APP_REGION_BASE) != FLASH_DATA_INVAL)
    {
        ENTER_CRITICAL();
        bsp_flash_write_nv(ACTION_JUMP_TO_APP);
        sys_jump_addr(FLASH_APP_REGION_BASE);
        EXIT_CRITICAL();
    }
}

static void dfu_recv_data_timeout(void)
{
    if (g_dfu.up_stage == UPGRADE_ING)
    {
        memset(rx_buff, 0, BUFF_SIZE);
    }
}

static void dfu_send_ack(void)
{
    uint8_t data_buf[6];
    uint16_t crc_val;

    data_buf[0] = rx_buff[0];                               // PACK_HEAD_1ST_BYTE
    data_buf[1] = rx_buff[1];                               // PACK_OPCODE
    data_buf[2] = rx_buff[2];                               // PACK_ID
    data_buf[3] = 0;                                        // PACK_DATA_LEN
    crc_val     = crc16(DFU_CRC16_INIT_VAL, data_buf, 4);
    data_buf[4] = (crc_val & 0x00ff);                       // PACK_CRC_LOW
    data_buf[5] = (crc_val >> 8) & 0x00ff;                  // PACK_CRC_HIGH
    bsp_uart_transmit(BSP_UART0, data_buf, 6, 0);
}

static void dfu_timer_restart(void)
{
    data_rx_timeout = 0;
}

static void dfu_recv_data_and_verify(bsp_uart_t uart, uint8_t *data, uint16_t len)
{
    uint16_t calc_crc;
    uint16_t recv_crc;
    uint16_t pdu_len = 0; //Protocol Data Unit
    uint16_t pack_len = 0;
    static uint8_t old_pack_id = 0xFF;

    dfu_timer_restart();
    memcpy(rx_buff, data, BUFF_SIZE);
    // Calculate the current packet length
    pdu_len = GET_DATA_LEN(rx_buff) + PACK_HEADER_LEN;
    pack_len = pdu_len + PACK_CRC_LEN;
    if(pack_len != len)
    {
        return;
    }

    if ((rx_buff[0] == DFU_PACK_HEAD_1ST_BYTE) &&
        (rx_buff[1] == DFU_PACK_OPCODE_DATA))
    {
        calc_crc = crc16(DFU_CRC16_INIT_VAL, rx_buff, pdu_len);
        recv_crc = (rx_buff[pack_len - 1] << 8) | rx_buff[pack_len - 2];
        if ((calc_crc == recv_crc) && (g_dfu.data_rx_stage == RX_DATA_ING)
            && (GET_PACK_ID(rx_buff) != old_pack_id))
        {
            old_pack_id = GET_PACK_ID(rx_buff);
            dfu_send_ack();
            memcpy(&flash_buf[g_dfu.rx_pack_num * PACK_DATA_LEN],
                    &GET_DATA_HEAD_ADDR(rx_buff),
                    GET_DATA_LEN(rx_buff));
            g_dfu.rx_pack_num ++;
            // After receiving 16 times of 2K, write flash once
            if (g_dfu.rx_pack_num >= PACK_MAX_PAGE)
            {
                g_dfu.rx_pack_num    = 0;
                g_dfu.data_rx_stage  = RX_DATA_END;
                g_dfu.up_stage       = UPGRADE_ING;
            }
            else if ((pack_len == PACK_HEADER_LEN + PACK_CRC_LEN) &&
                     (GET_DATA_LEN(rx_buff) == 0))
            {
                g_dfu.rx_pack_num    = 0;
                g_dfu.data_rx_stage  = RX_DATA_END;
                g_dfu.up_stage       = UPGRADE_END;
            }
        }
        memset(rx_buff, 0x00, sizeof(rx_buff));
    }
}

int32_t dfu_carry_app(void)
{
    uint32_t size = 0;
    int32_t ret = 0;

    size = ((bsp_flash_read_nv() >> 8) & 0x00ff) * FLASH_PAGE_SIZE;

    ENTER_CRITICAL();
    ret = bsp_flash_carry(FLASH_APP_REGION_BASE, FLASH_OTA_REGION_BASE, size);
    EXIT_CRITICAL();
    return ret;
}

static void dfu_init(void)
{
    ENTER_CRITICAL();
    bsp_flash_write_nv(ACTION_CLS_FLAG);
    EXIT_CRITICAL();
    bsp_uart_register_recv_callback(BSP_UART0, dfu_recv_data_and_verify);
    bsp_uart_init(BSP_UART0);
    bsp_uart_transmit(BSP_UART0, (uint8_t *)ENTER_DFU_MODE, strlen(ENTER_DFU_MODE), 0);
}

void enter_dfu_mode(void)
{
    int32_t ret = 0;
    uint32_t write_ptr = 0;

    dfu_init();

    while (1)
    {
        if (g_dfu.data_rx_stage == RX_DATA_END)
        {
            write_ptr = FLASH_APP_REGION_BASE +
                        g_dfu.pages_num * FLASH_PAGE_SIZE;
            ENTER_CRITICAL();
            bsp_flash_erase_page(write_ptr);
            ret = bsp_flash_write_words(write_ptr,
                                        (uint32_t *)flash_buf,
                                        ARRAY_SIZE(flash_buf));
            EXIT_CRITICAL();
            dfu_timer_restart();
            if (ret == RETVAL(E_OK))
            {
                g_dfu.pages_num += 1;
            }

            g_dfu.data_rx_stage = RX_DATA_ING;
            if (g_dfu.up_stage == UPGRADE_END)
            {
                g_dfu.up_stage = UPGRADE_FREE;
                dfu_jump_app();
            }
        }
        if (++data_rx_timeout >= RECV_TIMEOUT)
        {
            dfu_recv_data_timeout();
            dfu_timer_restart();
        }
    }
}
