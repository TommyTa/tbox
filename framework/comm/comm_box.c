/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file comm_box.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include <stdint.h>
#include <string.h>

#include "errorno.h"
#include "modules.h"

#include "comm_box.h"

typedef struct {
    const char* name;
    chan_msg_cbk_t callback;
} comm_box_msg_t;

static void comm_box_task(const void *arg);

SYS_THREAD_DEF(comm_box_task, SYS_PRIORITY_ABOVE_NORMAL, 400);
SYS_MSG_QDEF(comm_box, 3, uint32_t);
sys_msg_id_t g_comm_box_msg_id;

static comm_box_msg_t g_comm_box_chan[4];
static uint8_t g_comm_box_chan_num = 0;

void comm_box_register_msg_channel(const char* name, chan_msg_cbk_t cb)
{
    g_comm_box_chan[g_comm_box_chan_num].name = name;
    g_comm_box_chan[g_comm_box_chan_num].callback = cb;
    g_comm_box_chan_num ++;
}

int32_t comm_box_send_msg(uint8_t *buff, uint16_t len)
{
    comm_hdr_t *p_hdr;

    p_hdr = (comm_hdr_t*)buff;
    if (p_hdr->response == 1)
    {
        uint8_t swap = p_hdr->src;
        p_hdr->src = p_hdr->dst;
        p_hdr->dst = swap;
    }

    return comm_forward_msg(NULL, buff, len);
}

static void comm_box_msg_handle(uint8_t* buff, uint16_t len)
{
    uint8_t i = 0;

    for (i = 0; i < g_comm_box_chan_num; i++)
    {
        if (strncmp((char*)(buff + sizeof(comm_hdr_t)), g_comm_box_chan[i].name,
            strlen(g_comm_box_chan[i].name)) == 0)
        {
            g_comm_box_chan[i].callback(buff, len);
        }
    }
}

static void comm_box_task(const void *arg)
{
    sys_event_t evt;
    comm_msg_t *p_msg;

    while (1)
    {
        evt = sys_msg_get(g_comm_box_msg_id, SYS_WAIT_FOREVER);
        if (evt.status == SYS_EVENT_MESSAGE)
        {
            p_msg = (comm_msg_t*)evt.value.v;
        }
        else
        {
            continue;
        }

        comm_box_msg_handle(p_msg->buff, p_msg->len);
        mem_free(p_msg->buff);
        mem_free(p_msg);
    }
}

static void comm_box_task_init(void)
{
    sys_thread_create(SYS_THREAD(comm_box_task), NULL);
    g_comm_box_msg_id = sys_msg_create(SYS_MSG_Q(comm_box), NULL);
    comm_register_node('B', g_comm_box_msg_id);
}

SUBSYS_INITCALL("comm_box", comm_box_task_init);
