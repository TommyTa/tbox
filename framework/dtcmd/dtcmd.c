/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file dtcmd.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/
#include <string.h>
#include <stdint.h>

#include "modules.h"
#include "comm_box.h"
#include "dtcmd.h"
#include "atcmd.h"
#include "errorno.h"
#include "crc16.h"

#define DTCMD_DATA_HEADER           0xBBCCDDEE
#define DTCMD_DATA_CRC16_INIT_VAL   0xFFFF

#define DTCMD_NUM                   20
#define DTCMD_MAX_LEN               30

static comm_hdr_t comm_hdr;

char dt_cmd[DTCMD_NUM][DTCMD_MAX_LEN];
uint8_t dtcmd_register_num = 0;

static dtcmd_handle_cb_t dtcmd_handle_cb[DTCMD_NUM] = { NULL };

typedef struct 
{
    uint32_t header;
    uint16_t len;
    uint16_t crc16;
} dtcmd_hdr_t;

static dtcmd_hdr_t dtcmd_hdr;

void dtcmd_send_response(char* result, const char* str, uint16_t len)
{
    uint8_t *p_msg_buf;
    uint16_t msg_len = 0;
    uint16_t offset = 0;

    if (result == DTCMD_RESP_DATA)
    {
        msg_len = sizeof(comm_hdr) + sizeof(dtcmd_hdr) + len;
    }
    else
    {
        msg_len = sizeof(comm_hdr) + len + strlen(result);
    }
    p_msg_buf = mem_malloc(msg_len);
    if (!p_msg_buf)
    {
        return;
    }

    memcpy(p_msg_buf, &comm_hdr, sizeof(comm_hdr));
    offset += sizeof(comm_hdr);

    if (result == DTCMD_RESP_DATA)
    {
        dtcmd_hdr.header = DTCMD_DATA_HEADER;
        dtcmd_hdr.crc16 = crc16(DTCMD_DATA_CRC16_INIT_VAL, (uint8_t*)str, len);
        dtcmd_hdr.len = len;
        memcpy(p_msg_buf + offset, &dtcmd_hdr, sizeof(dtcmd_hdr_t));
        offset += sizeof(dtcmd_hdr_t);
    }

    memcpy(p_msg_buf + offset, str, len);
    offset += len;

    if (result != DTCMD_RESP_DATA)
    {
        memcpy(p_msg_buf + offset, result, strlen(result));
    }

    comm_box_send_msg(p_msg_buf, msg_len);
    mem_free(p_msg_buf);
}

static int32_t dtcmd_match(uint8_t *buf, uint8_t len)
{
    uint16_t i;

    for (i = 0; i < dtcmd_register_num; i ++)
    {
        if (strncmp((char*)buf, &dt_cmd[i][0], strlen(&dt_cmd[i][0])) == 0)
        {
            if (len == strlen(&dt_cmd[i][0]))
            {
                return i;
            }
            else
            {
                return RETVAL(E_INVAL_LEN);
            }
        }
    }

    return RETVAL(E_INVAL_DATA);
}

static void dtcmd_handle(int32_t cmd)
{
    switch (cmd)
    {
        case RETVAL(E_INVAL_DATA):
            dtcmd_send_response(DT_ERROR, DT_STR_NO_CMD, DT_STR_LEN(DT_STR_NO_CMD));
        break;

        case RETVAL(E_INVAL_LEN):
            dtcmd_send_response(DT_ERROR, DT_STR_ERR_ARGC, DT_STR_LEN(DT_STR_ERR_ARGC));
        break;

        default:
            (*dtcmd_handle_cb[cmd])();
        break;
    }
}

static void dtcmd_msg_handle(uint8_t* buff, uint16_t len)
{
    uint16_t        offset = 0;
    int32_t         match_result = 0;

    memcpy(&comm_hdr, buff, sizeof(comm_hdr));
    comm_hdr.response = 1;

    offset = sizeof(comm_hdr);
    match_result = dtcmd_match(buff + offset, len - offset);
    dtcmd_handle(match_result);
}

int32_t dtcmd_register(char *str, dtcmd_handle_cb_t cb)
{
    strcpy(&dt_cmd[dtcmd_register_num][0], str);
    if (dtcmd_handle_cb[dtcmd_register_num] != NULL)
    {
        return RETVAL(E_FAIL);
    }
    dtcmd_handle_cb[dtcmd_register_num] = cb;
    dtcmd_register_num ++;

    return RETVAL(E_OK);
}

static void dtcmd_init(void)
{
    comm_box_register_msg_channel("DT^", dtcmd_msg_handle);
    memset(&dt_cmd[0][0], 0, sizeof(dt_cmd));
}

SUBSYS_INITCALL("dtcmd", dtcmd_init);
