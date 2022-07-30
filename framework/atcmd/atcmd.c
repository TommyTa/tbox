/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file atcmd.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <string.h>
#include <stdint.h>

#include "modules.h"
#include "comm_box.h"
#include "x_strtok.h"

#include "atcmd.h"

extern atcmd_info_t __init_atcmd_start;
extern atcmd_info_t __init_atcmd_end;

static comm_hdr_t hdr;

void atcmd_send_response(char* result, const char* str, uint16_t len)
{
    uint8_t *p_msg_buf;
    uint16_t msg_len = 0;
    uint16_t offset = 0;

    msg_len = len + sizeof(hdr) + strlen(result);
    p_msg_buf = mem_malloc(msg_len);
    if (!p_msg_buf)
    {
        return;
    }

    memcpy(p_msg_buf, &hdr, sizeof(hdr));
    offset += sizeof(hdr);
    memcpy(p_msg_buf + offset, str, len);
    offset += len;
    memcpy(p_msg_buf + offset, result, strlen(result));

    comm_box_send_msg(p_msg_buf, msg_len);
    mem_free(p_msg_buf);
}

static void atcmd_msg_handle(uint8_t* buff, uint16_t len)
{
    atcmd_info_t   *atcmd;
    uint16_t        offset = 0;
    uint8_t         match = 0;
    char           *delim = " ,\r\n";
    char           *token_r;
    char           *argv[10];
    char           *arg;
    uint8_t         argc = 0;
    char            sbuf[256];

    memcpy(&hdr, buff, sizeof(hdr));
    hdr.response = 1;

    offset = sizeof(hdr);
    for (atcmd = &__init_atcmd_start; atcmd < &__init_atcmd_end; atcmd ++)
    {
        if (strncmp((char*)(buff + offset), atcmd->name, strlen(atcmd->name)) == 0)
        {
            match = 1;
            break;
        }
    }

    if (match)
    {
        offset += strlen(atcmd->name);
        memcpy(sbuf, (char*)(buff + offset), (len - offset));
        sbuf[len - offset] = '\0';

        arg = x_strtok_s(sbuf, delim, &token_r);
        if (arg)
        {
            argv[argc ++] = arg;
            while ((arg = x_strtok_s(NULL, delim, &token_r)) != NULL)
            {
                argv[argc ++] = arg;
            }
            atcmd->callback(argv, argc);
        }
        else
        {
            atcmd->callback(NULL, 0);
        }
    }
    else
    {
        atcmd_send_response(AT_ERROR, AT_STR_NO_CMD, AT_STR_LEN(AT_STR_NO_CMD));
    }
}

static void atcmd_init(void)
{
    comm_box_register_msg_channel("AT^", atcmd_msg_handle);
}

SUBSYS_INITCALL("atcmd", atcmd_init);
