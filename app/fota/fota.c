/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file fota.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crc16.h"
#include "errorno.h"
#include "modules.h"
#include "partition.h"

#include "atcmd.h"
#include "comm_box.h"
#include "sys_cmsis.h"

#include "bsp_flash.h"
#include "bsp_uart.h"

#define FOTA_PACK_HEAD_1ST_BYTE                 0xA5
#define FOTA_PACK_OPCODE_DATA                   0x5A
#define FOTA_PACK_OPCODE_ABORT                  0xB6
#define FOTA_GET_PACK_ID(BUFF)                  (BUFF[2])
#define FOTA_GET_DATA_LEN(BUFF)                 (BUFF[3])
#define FOTA_GET_DATA_HEAD_ADDR(BUFF)           (BUFF[4])

#define FOTA_CRC16_INIT_VAL                     0xFFFF
#define FLASH_PAGE_SIZE                         2048
#define RECV_TIMEOUT_MS                         3000
#define BUFF_SIZE                               256
#define PACK_DATA_LEN                           128
#define PACK_MAX_PAGE                           16
#define PACK_HEADER_LEN                         4
#define PACK_CRC_LEN                            2

#define ACTION_CLS_FLAG                         0
#define ACTION_CARRY_TO_APP                     2

#define FOTA_STR_LEN(str)                       strlen(str)
#define FOTA_RX_EVENT                           1

#define ATCMD_RET_CAN_UPGRADED                  "Can Upgraded\r\n"
#define ATCMD_RET_CANT_UPGRADED                 "Can't Upgraded\r\n"

static void fota_timer_handle(void const *arg);
static void fota_opcode_abort(void);

sys_timer_id_t fota_timer_id;

SYS_TIMER_DEF(fota_timer, fota_timer_handle);

typedef enum
{
    UPGRADE_FREE = 0,
    UPGRADE_ING,
    UPGRADE_END
} fota_up_stage_t;

typedef enum
{
    RX_DATA_ING = 0,
    RX_DATA_END
} fota_rx_stage_t;

typedef struct
{
    fota_up_stage_t  up_stage;
    fota_rx_stage_t  data_rx_stage;
    uint8_t          pages_num;
    uint16_t         pack_num;
    uint32_t         size_num;
} ota_t;
ota_t g_ota;

static comm_hdr_t hdr;

static uint8_t g_rx_buff[BUFF_SIZE];
static uint8_t g_flash_buf[FLASH_PAGE_SIZE];
static char g_msg_head[] = "UP^";
static char g_msg_tail[] = "\r\n";

static uint8_t ota_region_erased = 0;
static uint8_t g_pack_id = 0;
static uint8_t g_recv_timeout_flag = 0;

static void fota_jump_carry_mode(uint8_t carry_page_num)
{
    //high 8 page num
    bsp_flash_write_nv((carry_page_num << 8) | ACTION_CARRY_TO_APP);
    sys_reset();
}

static void fota_timer_start(void)
{
    sys_timer_start(fota_timer_id, RECV_TIMEOUT_MS);
}

static void fota_timer_stop(void)
{
    sys_timer_stop(fota_timer_id);
}

static void fota_recv_data_timeout(void)
{
    if (g_ota.up_stage == UPGRADE_ING)
    {
        memset(g_rx_buff, 0, BUFF_SIZE);
        fota_opcode_abort();
    }
}

static void fota_send_ack(char* result, char* err, const char* str, uint16_t len)
{
    uint8_t *p_msg_buf;
    uint16_t msg_len = 0;
    uint16_t offset = 0;

    msg_len = sizeof(hdr) + strlen(g_msg_head) + strlen(result) +
              strlen(err) + strlen(",") + len + strlen(g_msg_tail);
    p_msg_buf = mem_malloc(msg_len);
    if (!p_msg_buf)
    {
        return;
    }

    memcpy(p_msg_buf, &hdr, sizeof(hdr));
    offset += sizeof(hdr);
    memcpy(p_msg_buf + offset, g_msg_head, strlen(g_msg_head));
    offset += strlen(g_msg_head);
    memcpy(p_msg_buf + offset, result, strlen(result));
    offset += strlen(result);
    memcpy(p_msg_buf + offset, err, strlen(err));
    offset += strlen(err);
    memcpy(p_msg_buf + offset, ",", strlen(","));
    offset += strlen(",");
    memcpy(p_msg_buf + offset, str, len);
    offset += len;
    memcpy(p_msg_buf + offset, g_msg_tail, strlen(g_msg_tail));
    offset += strlen(g_msg_tail);

    comm_box_send_msg(p_msg_buf, msg_len);
    mem_free(p_msg_buf);
}

static void fota_opcode_data(uint8_t pack_len)
{
    memcpy(&g_flash_buf[g_ota.pack_num * PACK_DATA_LEN],
           &FOTA_GET_DATA_HEAD_ADDR(g_rx_buff),
            FOTA_GET_DATA_LEN(g_rx_buff));
    g_ota.size_num += FOTA_GET_DATA_LEN(g_rx_buff);
    g_ota.pack_num ++;
    // After receiving 16 times of 2K, write flash once
    if ((pack_len == (PACK_HEADER_LEN + PACK_CRC_LEN)) &&
        (FOTA_GET_DATA_LEN(g_rx_buff) == 0))
    {
        g_ota.pack_num = 0;
        g_ota.data_rx_stage = RX_DATA_END;
        g_ota.up_stage = UPGRADE_END;
    }
    else if (g_ota.pack_num >= PACK_MAX_PAGE)
    {
        g_ota.pack_num = 0;
        g_ota.data_rx_stage = RX_DATA_END;
        g_ota.up_stage = UPGRADE_ING;
    }
}

static void fota_opcode_abort(void)
{
    memset(&g_ota, 0, sizeof(ota_t));
    g_ota.pages_num = 0;
    ota_region_erased = 0;
    g_pack_id = 0;
    g_recv_timeout_flag = 0;
    fota_recv_data_timeout();
    memset(g_flash_buf, 0, FLASH_PAGE_SIZE);
}

static void fota_verify_data(bsp_uart_t uart, uint8_t *data, uint16_t len)
{
    int32_t ret = RETVAL(E_OK);
    uint16_t calc_crc;
    uint16_t recv_crc;
    uint8_t pdu_len = 0; // Protocol Data Unit
    uint8_t pack_len = 0;
    char pack_id_s[5];
    char err_s[5];

    (void)uart;

    g_recv_timeout_flag = 1;

    memcpy(g_rx_buff, data, len);

    // Calculate the current packet length
    if (len > 4)
    {
        pdu_len = FOTA_GET_DATA_LEN(g_rx_buff) + PACK_HEADER_LEN;
    }
    else
    {
        pdu_len = 0;
        return;
    }

    pack_len = pdu_len + PACK_CRC_LEN;
    if (len != pack_len)
    {
        ret = RETVAL(E_INVAL_LEN);
    }
    if ((ret == RETVAL(E_OK)) && g_rx_buff[0] == FOTA_PACK_HEAD_1ST_BYTE)
    {
        calc_crc = crc16(FOTA_CRC16_INIT_VAL, g_rx_buff, pdu_len);
        recv_crc = (g_rx_buff[pack_len - 2] << 8) | g_rx_buff[pack_len - 1];

        if (calc_crc == recv_crc)
        {
            if (g_rx_buff[1] == FOTA_PACK_OPCODE_DATA &&
                (g_ota.data_rx_stage == RX_DATA_ING))
            {
                // Verify that the packet ID is correct
                if ((FOTA_GET_PACK_ID(g_rx_buff) - g_pack_id == 1) ||
                    (g_pack_id - FOTA_GET_PACK_ID(g_rx_buff) == 255))
                {
                    fota_opcode_data(pack_len);
                }
                // Resend the acknowledge signal
                else if (FOTA_GET_PACK_ID(g_rx_buff) == g_pack_id)
                {
                    ret = RETVAL(E_OK);
                }
                else
                {
                    ret = RETVAL(E_MSG);
                }
            }
            else if (g_rx_buff[1] == FOTA_PACK_OPCODE_ABORT)
            {
                fota_opcode_abort();
                fota_timer_stop();
            }
            else
            {
                ret = RETVAL(E_MSG);
            }
        }
        else
        {
            ret = RETVAL(E_CRC);
        }
    }
    else
    {
        ret = RETVAL(E_INVAL_ADDR);
    }

    if (ret != RETVAL(E_OK))
    {
        itoa((int)FOTA_GET_PACK_ID(g_rx_buff), pack_id_s, 10);
        itoa((int)ret, err_s, 10);
        fota_send_ack("FAIL,", err_s, pack_id_s, FOTA_STR_LEN(pack_id_s));
    }
    else
    {
        g_pack_id = FOTA_GET_PACK_ID(g_rx_buff);
        itoa((int)g_pack_id, pack_id_s, 10);
        fota_send_ack("OK,", "0", pack_id_s, FOTA_STR_LEN(pack_id_s));
        if (g_rx_buff[1] == FOTA_PACK_OPCODE_ABORT)
        {
            g_pack_id = 0;
        }
    }
    memset(g_rx_buff, 0x00, sizeof(g_rx_buff));
}

static void fota_data_Handle(uint8_t *buff, uint16_t len)
{
    uint8_t offset;
    int32_t ret = RETVAL(E_OK);
    uint8_t page_num = 0;
    uint32_t write_ptr = 0;

    memcpy(&hdr, buff, sizeof(hdr));
    hdr.response = 1;

    offset = sizeof(hdr);
    offset += FOTA_STR_LEN(g_msg_head);

    fota_verify_data(BSP_UART0, (uint8_t *)(buff + offset),
                     len - offset);

    if (g_ota.data_rx_stage == RX_DATA_END)
    {
        write_ptr = FLASH_OTA_REGION_BASE +
                    g_ota.pages_num * FLASH_PAGE_SIZE;
        sys_irq_disable();
        // the whole ota region will be erased if the first dfu pack received
        if (ota_region_erased == 0)
        {
            uint8_t pages = FLASH_APP_REGION_SIZE / FLASH_PAGE_SIZE;
            uint8_t i;
            for (i = 0; i < pages; i++)
            {
                bsp_flash_erase_page(FLASH_OTA_REGION_BASE + i * FLASH_PAGE_SIZE);
            }
            ota_region_erased = 1;
            fota_timer_start();
        }
        ret = bsp_flash_write_words(write_ptr, (uint32_t *)g_flash_buf,
                                    ARRAY_SIZE(g_flash_buf));
        sys_irq_enable();
        if (ret == RETVAL(E_OK))
        {
            g_ota.pages_num += 1;
        }

        g_ota.data_rx_stage = RX_DATA_ING;
        if (g_ota.up_stage == UPGRADE_END)
        {
            g_ota.up_stage = UPGRADE_FREE;
            page_num = (g_ota.size_num % FLASH_PAGE_SIZE) == 0 ?
                        g_ota.size_num / FLASH_PAGE_SIZE :
                        g_ota.size_num / FLASH_PAGE_SIZE + 1;
            sys_irq_disable();
            fota_jump_carry_mode(page_num);
            sys_reset();
            sys_irq_enable();
        }
    }
}

static void fota_timer_handle(void const *arg)
{
    if (g_recv_timeout_flag)
    {
        g_recv_timeout_flag = 0;
    }
    else
    {
        fota_recv_data_timeout();
    }
}

static void fota_task_init(void)
{
    comm_box_register_msg_channel("UP^", fota_data_Handle);
    fota_timer_id = sys_timer_create(SYS_TIMER(fota_timer),
                                     SYS_TIMER_PERIODIC, NULL);

    memset(&g_ota,      0, sizeof(ota_t));
    memset(g_rx_buff,   0, BUFF_SIZE);
    memset(g_flash_buf, 0, FLASH_PAGE_SIZE);
}

static int32_t check_upgrade_state(char* argv[], int argc)
{
    uint8_t flag = 0;
    // check state

    if (flag == 0)
    {
        atcmd_send_response(AT_OK, ATCMD_RET_CAN_UPGRADED,
                            strlen(ATCMD_RET_CAN_UPGRADED));
    }
    else
    {
        atcmd_send_response(AT_ERROR, ATCMD_RET_CANT_UPGRADED,
                            strlen(ATCMD_RET_CANT_UPGRADED));
    }

    return RETVAL(E_OK);
}

APP_INITCALL("fota_task", fota_task_init);
ATCMD_INIT("AT^CHECKUP?", 0, check_upgrade_state);
