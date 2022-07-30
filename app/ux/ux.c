/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file ux.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "x_strtok.h"
#include "crc16.h"
#include "errorno.h"
#include "modules.h"
#include "partition.h"

#include "atcmd.h"
#include "comm_box.h"
#include "sys_cmsis.h"
#include "ui_box_led.h"
#include "ui_ear_led.h"
#include "ux.h"

#include "log.h"

#define UX_OK                                   "\r\nOK\r\n"
#define UX_ERROR                                "\r\nERROR\r\n"
#define UX_STR_LEN(str)                         strlen(str)
#define UX_CRC16_INIT_VAL                       0xFFFF
#define UX_CRC_LEN                              2
#define UX_MAC_LEN                              6
#define UX_BOX_VERSION_INFO_LEN                 4
#define UX_TEST                                 1
#define UX_VERSION_FACTORY                      0
#define UX_VERSION_ENGINEER                     1
#define UX_VERSION_USER                         2
#define UX_VERSION_ERR                          0xFF
#define UX_HEARTBEAT_TIME_MS                    500
#define UX_DISABLE                              0
#define UX_ENABLE                               1
#define UX_BOX_CLOSE                            0
#define UX_BOX_OPEN                             1
#define UX_ERA_OUT_BOX                          0
#define UX_ERA_IN_BOX                           1
#define UX_ERA_NO_PAIR                          0
#define UX_ERA_PAIR_ING                         1
#define UX_TIMEOUT_TIMES                        2

typedef struct
{
    uint8_t box_version_info[4];   // 000 000 000 000
    uint8_t box_version_type;      // 0--factory 1--engineer 2--user
    uint8_t box_battery;
    uint8_t left_ear_addr[6];
    uint8_t right_ear_addr[6];
    uint8_t left_ear_inbox;        // 0--Outbox 1--Inbox
    uint8_t right_ear_inbox;       // 0--Outbox 1--Inbox
    uint8_t box_state;             // 0--Close  1--Open
    uint8_t ear_pair_signal;       // ind pair
    uint8_t ear_reset_signal;      // Do not clear bone voiceprint pairing information
    uint8_t ear_reset_all_signal;  // Remove all
    uint8_t ear_reboot_signal;
    uint8_t ear_low_power_signal;
    uint8_t crc16[2];
} box_status_ind_t;

typedef struct
{
    uint8_t mac[6];
    uint8_t side;                  // L/R judge: 0--NULL  'L' 'R'
    uint8_t battery;
    uint8_t state;                 // 0--idle 1--bus
    uint8_t couple_state;          // 0--not  1--couple
    uint8_t pair_state;            // 0--not  1--ing
    uint8_t crc16[2];
} ear_status_ind_t;

typedef struct
{
    uint32_t signal;
} ux_event_msg_t;

static void ux_event_task(void const *argument);
static void ux_heartbeat_timer_handle(void const *argument);

static sys_msg_id_t ux_event_msg_id;
SYS_MSG_QDEF(ux_event_msg, 5, uint32_t);
SYS_THREAD_DEF(ux_event_task, SYS_PRIORITY_ABOVE_NORMAL, 200);

sys_timer_id_t ux_heartbeat_timer_id;
SYS_TIMER_DEF(ux_heartbeat_timer, ux_heartbeat_timer_handle);

static char g_msg_head[] = "UX^";
static char g_msg_tail[] = "\r\n";

static comm_hdr_t hdr;
static box_status_ind_t box_info;
static ear_status_ind_t left_ear_info;
static ear_status_ind_t right_ear_info;

static uint8_t g_ux_l_receive_timeout_flag = 0;
static uint8_t g_ux_r_receive_timeout_flag = 0;
static uint8_t g_show_era_reset_state = 0;

void ux_event_put_signal(uint32_t signal)
{
    ux_event_msg_t *p_msg = NULL;

    p_msg = mem_malloc(sizeof(ux_event_msg_t));
    if (p_msg != NULL)
    {
        p_msg->signal = signal;
        int32_t ret;
        ret = sys_msg_put(ux_event_msg_id, (uint32_t)p_msg, 0);
        if(ret != RETVAL(E_OK))
        {
            mem_free(p_msg);
        }
    }
}

static int32_t ux_analysis_ear_status_data(uint8_t *data, uint16_t len)
{
    ear_status_ind_t ear_info;
    int32_t ret = RETVAL(E_OK);
    uint16_t calc_crc;
    uint16_t recv_crc;

    if (len < sizeof(ear_status_ind_t))
    {
        ret = RETVAL(E_INVAL_LEN);
    }

    if (ret == RETVAL(E_OK))
    {
        memcpy(&ear_info, data, sizeof(ear_status_ind_t));
        calc_crc = crc16(UX_CRC16_INIT_VAL, (unsigned char *)&ear_info,
                                            sizeof(ear_status_ind_t) - UX_CRC_LEN);
        recv_crc = (ear_info.crc16[1] << 8) | (ear_info.crc16[0]);
        LOG_D("val:0x%02X 0x%02X 0x%02X\n", ear_info.battery, ear_info.state, ear_info.couple_state);
        if (calc_crc != recv_crc)
        {
            LOG_D("recv_crc:0x%02X 0x%02X \n", ear_info.crc16[1], ear_info.crc16[0]);
            LOG_D("calc_crc:0x%02X 0x%02X \n", (uint8_t)((calc_crc & 0xFF00) >> 8),
                                               (uint8_t)(calc_crc & 0x00ff));
            ret = RETVAL(E_FAIL);
        }
    }

    if (ret == RETVAL(E_OK))
    {
        if (ear_info.side == 'L')
        {
            memcpy(&left_ear_info, &ear_info, sizeof(ear_status_ind_t));
            memcpy(&(box_info.left_ear_addr), &(left_ear_info.mac), UX_MAC_LEN);
            box_info.left_ear_inbox = 1;
            if (g_ux_l_receive_timeout_flag >= UX_ENABLE)
            {
                g_ux_l_receive_timeout_flag = UX_DISABLE;
            }
            LOG_D("mac:0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
                  left_ear_info.mac[0], left_ear_info.mac[1], left_ear_info.mac[2],
                  left_ear_info.mac[3], left_ear_info.mac[4], left_ear_info.mac[5]);
        }
        else if (ear_info.side == 'R')
        {
            memcpy(&right_ear_info, &ear_info, sizeof(ear_status_ind_t));
            memcpy(&(box_info.right_ear_addr), &(right_ear_info.mac), UX_MAC_LEN);
            box_info.right_ear_inbox = 1;
            if (g_ux_r_receive_timeout_flag >= UX_ENABLE)
            {
                g_ux_r_receive_timeout_flag = UX_DISABLE;
            }
            LOG_D("mac:0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
                  right_ear_info.mac[0], right_ear_info.mac[1], right_ear_info.mac[2],
                  right_ear_info.mac[3], right_ear_info.mac[4], right_ear_info.mac[5]);
        }

        g_show_era_reset_state        = UX_DISABLE;
        box_info.ear_pair_signal      = UX_DISABLE;
        box_info.ear_reset_signal     = UX_DISABLE;
        box_info.ear_reset_all_signal = UX_DISABLE;
        box_info.ear_reboot_signal    = UX_DISABLE;

        if (box_info.box_state == UX_BOX_OPEN)
        {
            if (box_info.left_ear_inbox == UX_ERA_IN_BOX && box_info.right_ear_inbox == UX_ERA_IN_BOX)
            {
                if(left_ear_info.pair_state == UX_ERA_PAIR_ING)
                {
                    ui_show_ear_pair();
                }
                else
                {
                    uint8_t soc;
                    soc = (left_ear_info.battery > right_ear_info.battery) ?
                            left_ear_info.battery : right_ear_info.battery;
                    ui_show_ear_soc(soc);
                }
            }
            else if (box_info.left_ear_inbox == UX_ERA_IN_BOX)
            {
                ui_show_ear_soc(left_ear_info.battery);
            }
            else if (box_info.right_ear_inbox == UX_ERA_IN_BOX)
            {
                ui_show_ear_soc(right_ear_info.battery);
            }
            else
            {
                ui_ear_off_led();
            }
        }
    }

    return ret;
}

static int32_t ux_verify_data(uint8_t *data, uint16_t len)
{
    int32_t ret = RETVAL(E_OK);
    uint8_t offset;

    if ((strncmp((const char *)data, "INFO=", strlen("INFO=")) == 0))
    {
        offset = strlen("INFO=");
        sys_enter_critical();
        ux_analysis_ear_status_data((data + offset), len - offset);
        sys_exit_critical();
    }
    else
    {
        ret = RETVAL(E_INVAL_CMD);
    }

    return ret;
}

static void ux_data_Handle(uint8_t *buff, uint16_t len)
{
    uint8_t offset;

    memcpy(&hdr, buff, sizeof(comm_hdr_t));

    offset = sizeof(hdr);
    offset += UX_STR_LEN(g_msg_head);

    ux_verify_data((uint8_t *)(buff + offset), len - offset);
}

static void ux_send_data(char *cmd, const void *str, uint16_t len)
{
    uint8_t *p_msg_buf;
    uint16_t msg_len = 0;
    uint16_t offset = 0;

    msg_len = sizeof(hdr) + strlen(g_msg_head) + strlen(cmd) +
              len + strlen(g_msg_tail);
    p_msg_buf = mem_malloc(msg_len);
    if (!p_msg_buf)
    {
        return;
    }

    memcpy(p_msg_buf, &hdr, sizeof(hdr));
    offset += sizeof(hdr);
    memcpy(p_msg_buf + offset, g_msg_head, strlen(g_msg_head));
    offset += strlen(g_msg_head);
    memcpy(p_msg_buf + offset, cmd, strlen(cmd));
    offset += strlen(cmd);
    memcpy(p_msg_buf + offset, str, len);
    offset += len;
    memcpy(p_msg_buf + offset, g_msg_tail, strlen(g_msg_tail));
    offset += strlen(g_msg_tail);
    comm_box_send_msg(p_msg_buf, msg_len);

    mem_free(p_msg_buf);
}

static void ux_send_ind_cmd(uint8_t dst)
{
    uint16_t calc_crc;

    hdr.noprefix = 0;
    hdr.src = 'B';
    hdr.dst = dst;

    calc_crc = crc16(UX_CRC16_INIT_VAL, (unsigned char *)&box_info,
                     sizeof(box_status_ind_t) - UX_CRC_LEN);
    box_info.crc16[0] = (uint8_t)(calc_crc & 0x00FF);
    box_info.crc16[1] = (calc_crc & 0xFF00) >> 8;
    ux_send_data("IND=", &box_info, sizeof(box_status_ind_t));

}

static void ux_signal_event(uint8_t signal)
{
    switch (signal)
    {
        case UX_SIGNAL_BOX_OPEN:
            LOG_D("UX_SIGNAL_BOX_OPEN\r\n");
            box_info.box_state = UX_ENABLE;
            break;
        case UX_SIGNAL_BOX_CLOSE:
            LOG_D("UX_SIGNAL_BOX_CLOSE\r\n");
            box_info.box_state = UX_DISABLE;
            break;
        case UX_SIGNAL_BOX_LONG:
            if (box_info.box_state)
            {
                LOG_D("UX_SIGNAL_BOX_LONG\r\n");
                box_info.ear_pair_signal = UX_ENABLE;
            }
            break;
        case UX_SIGNAL_BOX_LONG_LONG:
            if (box_info.box_state)
            {
                LOG_D("UX_SIGNAL_BOX_LONG_LONG\r\n");
                box_info.ear_reset_signal = UX_ENABLE;
            }
            break;
        case UX_SIGNAL_BOX_FIVE_CLICK:
            if (box_info.box_state)
            {
                LOG_D("UX_SIGNAL_BOX_FIVE_CLICK\r\n");
                box_info.ear_reset_all_signal = UX_ENABLE;
            }
            break;
        default:
            break;
    }
}

static void ux_event_task(void const *argument)
{
    sys_event_t evt;
    ux_event_msg_t *p_msg = NULL;
    uint8_t signal;

    while (1)
    {
        evt = sys_msg_get(ux_event_msg_id, SYS_WAIT_FOREVER);
        if (evt.status == SYS_EVENT_MESSAGE)
        {
            p_msg = (ux_event_msg_t *)evt.value.v;
            signal = p_msg->signal;
            mem_free(p_msg);
            ux_signal_event(signal);
            ux_send_ind_cmd('L');
            ux_send_ind_cmd('R');
        }
        else
        {
            continue;
        }
    }
}

static void ux_receive_timeout(void)
{
    g_ux_l_receive_timeout_flag++;
    if (g_ux_l_receive_timeout_flag >= UX_TIMEOUT_TIMES)
    {
        g_ux_l_receive_timeout_flag = UX_TIMEOUT_TIMES;
        box_info.left_ear_inbox = UX_ERA_OUT_BOX;
    }

    g_ux_r_receive_timeout_flag++;
    if (g_ux_l_receive_timeout_flag >= UX_TIMEOUT_TIMES)
    {
        g_ux_l_receive_timeout_flag = UX_TIMEOUT_TIMES;
        box_info.left_ear_inbox = UX_ERA_OUT_BOX;
    }

    if (box_info.left_ear_inbox == UX_ERA_OUT_BOX && box_info.right_ear_inbox == UX_ERA_OUT_BOX &&
        box_info.box_state == UX_BOX_OPEN)
    {
        if (box_info.ear_reset_signal == UX_ENABLE ||
            box_info.ear_reset_all_signal == UX_ENABLE)
        {
            if (g_show_era_reset_state == UX_DISABLE)
            {
                g_show_era_reset_state = UX_ENABLE;
                ui_show_ear_reset();
            }
        }
    }
}

static void ux_heartbeat_timer_handle(void const *arg)
{
    sys_enter_critical();
    ux_receive_timeout();

    ux_send_ind_cmd('L');
    ux_send_ind_cmd('R');
    sys_exit_critical();
}

static void ux_heartbeat_timer_init(void)
{
    sys_status status;

    ux_heartbeat_timer_id = sys_timer_create(SYS_TIMER(ux_heartbeat_timer),
                                             SYS_TIMER_PERIODIC, NULL);
    if (ux_heartbeat_timer_id)
    {
        status = sys_timer_start(ux_heartbeat_timer_id, UX_HEARTBEAT_TIME_MS);
        if (status != RETVAL(E_OK))
        {
            ;
        }
    }
}

static void ux_set_box_version(void)
{
    char *p;
    char *token_r;
    char sbuf[] = REVISION_INFO_S;
    uint8_t buff[5];
    uint8_t i = 0;

    p = x_strtok_s(sbuf, ".", &token_r);
    while (p != NULL)
    {
        buff[i++] = (uint8_t)atoi(p);
        p = x_strtok_s(NULL, ".", &token_r);
    }
    memcpy(&(box_info.box_version_info), buff, UX_BOX_VERSION_INFO_LEN);

    if (strncmp((const char *)REVISION_TYPE_S, "factory", strlen("factory")) == 0)
    {
        box_info.box_version_type = UX_VERSION_FACTORY;
    }
    else if (strncmp((const char *)REVISION_TYPE_S, "engineer", strlen("engineer")) == 0)
    {
        box_info.box_version_type = UX_VERSION_ENGINEER;
    }
    else if (strncmp((const char *)REVISION_TYPE_S, "user", strlen("user")) == 0)
    {
        box_info.box_version_type = UX_VERSION_USER;
    }
    else
    {
        box_info.box_version_type = UX_VERSION_ERR;
    }
}

static void ux_task_init(void)
{
    comm_box_register_msg_channel("UX^", ux_data_Handle);
    ux_event_msg_id = sys_msg_create(SYS_MSG_Q(ux_event_msg), NULL);
    sys_thread_create(SYS_THREAD(ux_event_task), NULL);
    ux_heartbeat_timer_init();

    memset(&box_info,       0, sizeof(box_status_ind_t));
    memset(&left_ear_info,  0, sizeof(ear_status_ind_t));
    memset(&right_ear_info, 0, sizeof(ear_status_ind_t));

    ux_set_box_version();
}

/************************************************************************************/
#if UX_TEST
static int32_t ux_test_send_ind_l(char *argv[], int argc)
{
    ux_send_ind_cmd('L');
    atcmd_send_response(AT_OK, "send ux data L\r\n",
                        strlen("send ux data L\r\n"));
    return RETVAL(E_OK);
}

static int32_t ux_test_send_ind_r(char *argv[], int argc)
{
    ux_send_ind_cmd('R');
    atcmd_send_response(AT_OK, "send ux data R\r\n",
                        strlen("send ux data R\r\n"));
    return RETVAL(E_OK);
}

ATCMD_INIT("AT^UXIND?", 0, ux_test_send_ind_l);
ATCMD_INIT("AT^UXIND1?", 0, ux_test_send_ind_r);
#endif

APP_INITCALL("ux_task", ux_task_init);
