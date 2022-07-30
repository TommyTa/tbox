/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file comm_uart.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include <stdint.h>
#include <string.h>

#include "bsp_uart.h"
#include "bsp_flash.h"

#include "sys_cmsis.h"
#include "modules.h"
#include "comm_core.h"
#include "errorno.h"

static void comm_uart_task(const void *arg);

SYS_THREAD_DEF(comm_uart_task, SYS_PRIORITY_ABOVE_NORMAL, 400);

SYS_MSG_QDEF(comm_uart, 3, uint32_t);

sys_msg_id_t g_comm_uart_msg_id;

static void comm_uart_recv(bsp_uart_t uart, uint8_t *buff, uint16_t len)
{
    comm_hdr_t hdr;
    uint8_t match = 0;

    (void)uart;

    if (buff == NULL)
    {
        return;
    }

    memset(&hdr, 0 ,sizeof(hdr));

    if ((strncmp((const char *)buff, "AT^", strlen("AT^")) == 0) ||
        (strncmp((const char *)buff, "UX^", strlen("UX^")) == 0) ||
        (strncmp((const char *)buff, "UP^", strlen("UP^")) == 0) ||
        (strncmp((const char *)buff, "DT^", strlen("DT^")) == 0) ||
        (strncmp((const char *)buff, "\r\n", strlen("\r\n")) == 0) ||
        (strncmp((const char *)buff, "\r\nOK", strlen("\r\nOK")) == 0) ||
        (strncmp((const char *)buff, "\r\nERROR", strlen("\r\nERROR")) == 0))
    {
        hdr.noprefix = 1;
        hdr.src = 'C';
        hdr.dst = 'B';
        match = 1;
    }
    else if ((strncmp((const char *)buff, "CL", strlen("CL")) == 0) ||
             (strncmp((const char *)buff, "CR", strlen("CR")) == 0) ||
             (strncmp((const char *)buff, "CB", strlen("CB")) == 0) ||
             (strncmp((const char *)buff, "BL", strlen("BL")) == 0) ||
             (strncmp((const char *)buff, "BR", strlen("BR")) == 0))
    {
        hdr.noprefix = 0;
        match = 1;
    }
    else if ((strncmp((const char *)buff, "bootloader", strlen("bootloader")) == 0))
    {
        // 0--ACTION_JUMP_TO_APP  1--ACTION_UPGRADE_APP  2--ACTION_CARRY_TO_APP
        bsp_flash_write_nv(1);
        sys_reset();
    }

    if (match)
    {
        comm_forward_msg(&hdr, buff, len);
    }
}

static void comm_uart_task(const void *arg)
{
    sys_event_t evt;
    comm_msg_t *p_msg;
    uint8_t offset;

    while (1)
    {
        evt = sys_msg_get(g_comm_uart_msg_id, SYS_WAIT_FOREVER);
        if (evt.status == SYS_EVENT_MESSAGE)
        {
            p_msg = (comm_msg_t *)evt.value.v;
        }
        else
        {
            continue;
        }

        offset = sizeof(comm_hdr_t);
        sys_enter_critical();
        bsp_uart_transmit(BSP_UART0, (uint8_t *)(p_msg->buff + offset), p_msg->len - offset, 0xFFFFFFFF);
        sys_exit_critical();
        mem_free(p_msg->buff);
        mem_free(p_msg);
    }
}

void comm_uart_task_init(void)
{
    sys_thread_create(SYS_THREAD(comm_uart_task), NULL);
    g_comm_uart_msg_id = sys_msg_create(SYS_MSG_Q(comm_uart), NULL);
    comm_register_node('C', g_comm_uart_msg_id);
    bsp_uart_register_recv_callback(BSP_UART0 , comm_uart_recv);
}

SUBSYS_INITCALL("comm_uart", comm_uart_task_init);
