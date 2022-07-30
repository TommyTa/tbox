/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file comm_core.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include <string.h>

#include "errorno.h"
#include "comm_core.h"
#include "log.h"
#include "sys_cmsis.h"
#include "cmsis_os.h"
static comm_node_t g_comm_node[4];
static uint8_t g_comm_node_num = 0;

static comm_node_t* comm_find_node_by_name(uint8_t name)
{
    uint8_t i = 0;

    for (i = 0; i < g_comm_node_num; i++)
    {
        if (g_comm_node[i].name == name)
        {
            return &g_comm_node[i];
        }
    }

    return NULL;
}

static int32_t comm_send_msg(comm_node_t *node, uint8_t *buff, uint16_t len)
{
    comm_msg_t *p_msg;
    int32_t ret;

    p_msg = mem_malloc(sizeof(comm_msg_t));
    if (!p_msg)
    {
        return RETVAL(E_ALLOC);
    }
    p_msg->buff     = buff;
    p_msg->len      = len;

    ret = sys_msg_put(node->msg_id, (uint32_t)p_msg, 0);
    if(ret != RETVAL(E_OK))
    {
        mem_free(p_msg);
        return RETVAL(E_SEND);
    }
    LOG_D("ret:%d\r\n", ret);
    return RETVAL(E_OK);
}

int32_t comm_forward_msg(comm_hdr_t *p_hdr, uint8_t *buff, uint16_t len)
{
    uint8_t offset = 0;
    uint16_t msg_len = 0;
    uint8_t *p_msg_buf;
    comm_node_t *p_comm_node = NULL;

    if (p_hdr)
    {
        if (p_hdr->noprefix)
        {
            offset = sizeof(comm_hdr_t);
        }
        else
        {
            /* The size of the prefix is 2 bytes. */
            offset = 2;
        }
    }

    msg_len = offset + len;
    p_msg_buf = mem_malloc(msg_len);
    if (!p_msg_buf)
    {
        return RETVAL(E_ALLOC);
    }

    if (p_hdr)
    {
        memcpy(p_msg_buf, p_hdr, offset);
    }
    memcpy(p_msg_buf + offset, buff, len);

    if (((comm_hdr_t *)p_msg_buf)->response)
    {
        p_comm_node = comm_find_node_by_name(((comm_hdr_t *)p_msg_buf)->channel);
    }

    if (p_comm_node == NULL)
    {
        p_comm_node = comm_find_node_by_name(((comm_hdr_t *)p_msg_buf)->dst);
    }

    if (p_comm_node)
    {
        int32_t ret;
        ret = comm_send_msg(p_comm_node, p_msg_buf, msg_len);
        if (ret != 0)
        {
            mem_free(p_msg_buf);
            return RETVAL(E_SEND);
        }
    }
    else
    {
        mem_free(p_msg_buf);
        return RETVAL(E_FAIL);
    }

    return RETVAL(E_OK);
}

int32_t comm_register_node(uint8_t name, sys_msg_id_t msg_id)
{
    g_comm_node[g_comm_node_num].name = name;
    g_comm_node[g_comm_node_num].msg_id = msg_id;
    g_comm_node_num ++;

    return RETVAL(E_OK);
}
