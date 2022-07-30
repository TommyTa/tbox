/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file comm_left_ear.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include <stdint.h>
#include <string.h>

#include "bsp_uart.h"
#include "modules.h"

#include "comm_core.h"

static void comm_left_ear_task(const void *arg);
static void comm_right_ear_task(const void *arg);

SYS_THREAD_DEF(comm_left_ear_task, SYS_PRIORITY_NORMAL, 300);
SYS_MSG_QDEF(comm_left_ear, 3, uint32_t);
sys_msg_id_t g_comm_left_ear_msg_id;

SYS_THREAD_DEF(comm_right_ear_task, SYS_PRIORITY_BELOW_NORMAL, 300);
SYS_MSG_QDEF(comm_right_ear, 3, uint32_t);
sys_msg_id_t g_comm_right_ear_msg_id;

static void comm_ear_recv(bsp_uart_t uart, uint8_t *buff, uint16_t len)
{
    comm_hdr_t hdr;
    uint8_t match = 0;
    
    (void)uart;

    if (buff == NULL)
    {
        return;
    }

    memset(&hdr, 0 ,sizeof(hdr));

    if ((strncmp((const char *)buff, "RL", strlen("RL")) == 0) ||
        (strncmp((const char *)buff, "RB", strlen("RB")) == 0) ||
        (strncmp((const char *)buff, "RC", strlen("RC")) == 0) ||
        (strncmp((const char *)buff, "LR", strlen("LR")) == 0) ||
        (strncmp((const char *)buff, "LB", strlen("LB")) == 0) ||
        (strncmp((const char *)buff, "LC", strlen("LC")) == 0))
    {
        hdr.noprefix = 0;
        match = 1;
    }
    else if((strncmp((const char *)buff, "PB", strlen("PB")) == 0))
    {
        if(uart == BSP_UART1)
        {
            hdr.channel = 'L';
        }
        else if(uart == BSP_UART2)
        {
            hdr.channel = 'R';
        }
        hdr.noprefix = 0;
        match = 1;
    }
    if (match)
    {
        comm_forward_msg(&hdr, buff, len);
    }
}

static void comm_left_ear_task(const void *arg)
{
    sys_event_t evt;
    comm_msg_t *p_msg;
    uint8_t offset = 0;

    while (1)
    {
        evt = sys_msg_get(g_comm_left_ear_msg_id, SYS_WAIT_FOREVER);
        if (evt.status == SYS_EVENT_MESSAGE)
        {
            p_msg = (comm_msg_t *)evt.value.v;
        }
        else
        {
            continue;
        }
        
        /* The size of the prefix is 2 bytes. */
        offset = 2;
        sys_enter_critical();
        bsp_uart_transmit(BSP_UART1, (uint8_t *)(p_msg->buff + offset), p_msg->len - offset, 0xFFFFFFFF);
        sys_exit_critical();
        mem_free(p_msg->buff);
        mem_free(p_msg);
    }
}

static void comm_right_ear_task(const void *arg)
{
    sys_event_t evt;
    comm_msg_t *p_msg;
    uint8_t offset;

    while (1)
    {
        evt = sys_msg_get(g_comm_right_ear_msg_id, SYS_WAIT_FOREVER);
        if (evt.status == SYS_EVENT_MESSAGE)
        {
            p_msg = (comm_msg_t *)evt.value.v;
        }
        else
        {
            continue;
        }

        /* The size of the prefix is 2 bytes. */
        offset = 2;
        sys_enter_critical();
        bsp_uart_transmit(BSP_UART2, (uint8_t *)(p_msg->buff + offset), p_msg->len - offset, 0xFFFFFFFF);
        sys_exit_critical();
        mem_free(p_msg->buff);
        mem_free(p_msg);
    }
}

static void comm_tws_task_init(void)
{
    sys_thread_create(SYS_THREAD(comm_left_ear_task), NULL);
    g_comm_left_ear_msg_id = sys_msg_create(SYS_MSG_Q(comm_left_ear), NULL);
    comm_register_node('L', g_comm_left_ear_msg_id);
    bsp_uart_register_recv_callback(BSP_UART1 , comm_ear_recv);

    sys_thread_create(SYS_THREAD(comm_right_ear_task), NULL);
    g_comm_right_ear_msg_id = sys_msg_create(SYS_MSG_Q(comm_right_ear), NULL);
    comm_register_node('R', g_comm_right_ear_msg_id);
    bsp_uart_register_recv_callback(BSP_UART2 , comm_ear_recv);
}

SUBSYS_INITCALL("comm_tws", comm_tws_task_init);
