/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file trace.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "errorno.h"
#include "bsp_uart.h"
#include "sys_cmsis.h"
#include "modules.h"
#include "trace_area.h"
#include "atcmd.h"

#include "trace.h"

#define TRACE_LOG_BUF_NUM               2
#define TRACE_LOG_BUF_SIZE              (NVM_PAGE_SIZE / TRACE_LOG_BUF_NUM)

#define TRACE_LOG_BUF_AUTO_FLUSH_LEN    0

#define TRACE_AUTO_FLUSH
#ifdef TRACE_AUTO_FLUSH
#define TRACE_AUTO_FLUSH_TIMEOUT        10
#define TRACE_AUTO_FLUSH_TIMEOUT_1S     1000
#endif

#define TRACE_BUF_STS_BUSY              1
#define TRACE_BUF_STS_IDLE              0

#define TRACE_MAGIC                     0xB6

extern uint32_t __trc_str_start;
extern uint32_t __trc_str_end;

typedef struct 
{
#ifdef TRACE_AUTO_FLUSH
    sys_timer_id_t auto_flush_timer_id;
    uint16_t auto_flush_time_cnt_s;
    uint8_t auto_flush_remain_sts;
#endif
    uint8_t buf_idx;
    uint16_t buf_ofs;
    uint8_t buf_sts[TRACE_LOG_BUF_NUM];
    char buf[TRACE_LOG_BUF_NUM][TRACE_LOG_BUF_SIZE];
    uint32_t time_stamp_ms;
    sys_timer_id_t stamp_timer_id;
} trace_t;

typedef struct {
    uint32_t magic : 8;
    uint32_t argc  : 4;
    uint32_t level : 3;
    uint32_t addr  : 17;
    uint32_t timestamp;
} __attribute__((packed)) trace_head_t;

static trace_t g_trace;
static char trace_info[150] = "";
trace_area_t trace_area_tmp;

SYS_MUTEX_DEF(log_append_mutex);
sys_mutex_id_t log_append_mutex_id;


void trace_store_enable(void)
{
    trace_area_store_enable();
}

void trace_store_disable(void)
{
    trace_area_store_disable();
}

void trace_stop_read(void)
{
    trace_area_stop_read();
}

static uint16_t trace_get_write_buf_ofs(void)
{
    return g_trace.buf_ofs;
}

static uint8_t trace_get_write_buf_idx(void)
{
    return g_trace.buf_idx;
}

static uint8_t trace_get_next_buf_idx(uint8_t idx)
{
    if (idx)
    {
        return 0;
    }

    return 1;
}

static void trace_flush_buf(char *buf, uint8_t idx, uint16_t len)
{
    trace_area_store_buffer(buf, idx, len);
}

#ifdef TRACE_AUTO_FLUSH
void trace_auto_flush_timer_reset(void)
{
    g_trace.auto_flush_time_cnt_s = 0;
}

void trace_auto_flush(void)
{
    uint8_t curr_buf_idx = 0;
    uint8_t next_buf_idx = 0;

    curr_buf_idx = g_trace.buf_idx;
    next_buf_idx = trace_get_next_buf_idx(g_trace.buf_idx);

    g_trace.buf_sts[curr_buf_idx] = TRACE_BUF_STS_BUSY;
    trace_flush_buf(g_trace.buf[g_trace.buf_idx], curr_buf_idx, g_trace.buf_ofs);

    if (g_trace.buf_sts[next_buf_idx] == TRACE_BUF_STS_IDLE)
    {
        g_trace.buf_idx = next_buf_idx;
        g_trace.buf_ofs = 0;
    }
}

void trace_auto_flush_remain_sts_set(auto_flush_remain_sts_t sts)
{
    g_trace.auto_flush_remain_sts = sts;
}

auto_flush_remain_sts_t trace_auto_flush_remain_sts_get(void)
{
    return g_trace.auto_flush_remain_sts;
}

static void trace_auto_flush_timer_handle(void const *arg)
{
    g_trace.auto_flush_time_cnt_s ++;
    if (g_trace.auto_flush_time_cnt_s >= TRACE_AUTO_FLUSH_TIMEOUT)
    {
        if (g_trace.buf_ofs > TRACE_LOG_BUF_AUTO_FLUSH_LEN)
        {
            trace_auto_flush();

            trace_auto_flush_timer_reset();
        }
        else if (g_trace.buf_ofs == 0)
        {
            if (AUTO_FLUSH_REMAIN_STS_IN_PROCESS == trace_auto_flush_remain_sts_get())
            {
                trace_auto_flush_remain_sts_set(AUTO_FLUSH_REMAIN_STS_DONE);
            }
        }
    }
}

SYS_TIMER_DEF(trace_auto_flush_timer, trace_auto_flush_timer_handle);

static void trace_auto_flush_timer_init(void)
{
    sys_status status;

    g_trace.auto_flush_timer_id = sys_timer_create(SYS_TIMER(trace_auto_flush_timer),
                        SYS_TIMER_PERIODIC, NULL);
    if (g_trace.auto_flush_timer_id)
    {
        status = sys_timer_start(g_trace.auto_flush_timer_id, TRACE_AUTO_FLUSH_TIMEOUT_1S);
        if (status != RETVAL(E_OK))
        {
            ;
        }
    }
}

void trace_auto_flush_timer_set_ms(uint32_t time_ms)
{
    sys_status status;

    status = sys_timer_stop(g_trace.auto_flush_timer_id);
    if (status == RETVAL(E_OK))
    {
        sys_timer_start(g_trace.auto_flush_timer_id, time_ms);
    }
}

void trace_auto_flush_timer_restore(void)
{
    sys_status status;

    status = sys_timer_stop(g_trace.auto_flush_timer_id);
    if (g_trace.auto_flush_timer_id)
    {
        status = sys_timer_start(g_trace.auto_flush_timer_id, TRACE_AUTO_FLUSH_TIMEOUT_1S);
        if (status != RETVAL(E_OK))
        {
            ;
        }
    }
}
#endif

static void trace_stamp_timer_handle(void const *arg)
{
    g_trace.time_stamp_ms ++;
}

SYS_TIMER_DEF(trace_stamp_timer, trace_stamp_timer_handle);

static void trace_stamp_timer_init(void)
{
    sys_status status;

    g_trace.stamp_timer_id = sys_timer_create(SYS_TIMER(trace_stamp_timer),
                             SYS_TIMER_PERIODIC, NULL);
    if (g_trace.stamp_timer_id)
    {
        status = sys_timer_start(g_trace.stamp_timer_id, 1);
        if (status != RETVAL(E_OK))
        {
            ;
        }
    }
}

static int32_t trace_format_buf(uint32_t attr, char *buf, uint16_t size,
                                const char *fmt, va_list ap)
{
    trace_head_t trace_hdr;
    uint32_t value[10];
    uint8_t num;
    uint8_t level;
    uint8_t i;

    if (size < sizeof(trace_hdr) + sizeof(value))
    {
        return RETVAL(E_NO_SPACE);
    }

    num = GET_BITFIELD(attr, LOG_ATTR_ARG_NUM);
    if (num > 10)
    {
        num = 10;
    }
    level = GET_BITFIELD(attr, LOG_ATTR_LEVEL);

    for (i = 0; i < num; i++)
    {
        value[i] = va_arg(ap, uint32_t);
        if (value[i] == NVM_EMPTY_DATA)
        {
            value[i] = NVM_SPECIAL_DATA;
        }
    }

    trace_hdr.magic     = TRACE_MAGIC;
    trace_hdr.argc      = num;
    trace_hdr.level     = level;
    trace_hdr.addr      = (uint32_t)fmt - (uint32_t)(FLASH_OTA_REGION_BASE);
    trace_hdr.timestamp = g_trace.time_stamp_ms;

    memcpy(buf, &trace_hdr, sizeof(trace_hdr));
    if (num != 0)
    {
        memcpy(buf + sizeof(trace_hdr), value, sizeof(uint32_t)*num);
    }

    return (sizeof(trace_hdr) + sizeof(uint32_t)*num);
}

static void trace_store_complete(char *buf, uint8_t idx, uint16_t len)
{
    /* Set idle status */
    g_trace.buf_sts[idx] = TRACE_BUF_STS_IDLE;
    if (AUTO_FLUSH_REMAIN_STS_IN_PROCESS == trace_auto_flush_remain_sts_get())
    {
        trace_auto_flush_remain_sts_set(AUTO_FLUSH_REMAIN_STS_DONE);
    }
}

static void trace_buffer_append(char *buf, uint16_t len)
{
    uint16_t curr_buf_left = TRACE_LOG_BUF_SIZE - g_trace.buf_ofs;
    uint8_t curr_buf_idx = g_trace.buf_idx;
    uint8_t next_buf_idx = trace_get_next_buf_idx(g_trace.buf_idx);

    if ((g_trace.buf_ofs + len) >= TRACE_LOG_BUF_SIZE)
    {
        if ((g_trace.buf_sts[curr_buf_idx] == TRACE_BUF_STS_IDLE))
        {
            uint16_t left_len = 0;
            // Current log buffer
            memcpy(&g_trace.buf[curr_buf_idx][g_trace.buf_ofs], buf, curr_buf_left);
            // Set busy status
            g_trace.buf_sts[curr_buf_idx] = TRACE_BUF_STS_BUSY;
            // Flush current log buffer
            trace_flush_buf(&g_trace.buf[curr_buf_idx][0], curr_buf_idx, TRACE_LOG_BUF_SIZE);

            if (g_trace.buf_sts[next_buf_idx] == TRACE_BUF_STS_IDLE)
            {
                // Update the index of buffer
                g_trace.buf_idx = next_buf_idx;
                // Next log buffer
                left_len = len - curr_buf_left;
                if (left_len != 0)
                {
                    memcpy(&g_trace.buf[next_buf_idx][0], &buf[curr_buf_left], left_len);
                }
                g_trace.buf_ofs = left_len;
            }
            else
            {
                ;
            }
        }
        else if ((g_trace.buf_sts[next_buf_idx] == TRACE_BUF_STS_IDLE))
        {
            g_trace.buf_idx = next_buf_idx;
            g_trace.buf_ofs = 0;
            memcpy(&g_trace.buf[g_trace.buf_idx][g_trace.buf_ofs], buf, len);

            if (len == TRACE_LOG_BUF_SIZE)
            {
                // Set busy status
                g_trace.buf_sts[g_trace.buf_idx] = TRACE_BUF_STS_BUSY;
                // Flush current log buffer
                trace_flush_buf(g_trace.buf[curr_buf_idx], curr_buf_idx, TRACE_LOG_BUF_SIZE);
            }
        }
    }
    else
    {
        if ((g_trace.buf_sts[curr_buf_idx] == TRACE_BUF_STS_IDLE))
        {
            memcpy(&g_trace.buf[curr_buf_idx][g_trace.buf_ofs], buf, len);
            g_trace.buf_ofs += len;
        }
        else if ((g_trace.buf_sts[next_buf_idx] == TRACE_BUF_STS_IDLE))
        {
            g_trace.buf_idx = next_buf_idx;
            g_trace.buf_ofs = 0;
            memcpy(&g_trace.buf[g_trace.buf_idx][g_trace.buf_ofs], buf, len);
            g_trace.buf_ofs += len;
        }
    }
}

int32_t trace_dump_log(char* buf)
{
    trace_area_t trace_area;

    if ((g_trace.buf_sts[0] == TRACE_BUF_STS_BUSY) ||
        (g_trace.buf_sts[1] == TRACE_BUF_STS_BUSY))
    {
        return RETVAL(E_BUSY);
    }

    trace_area_get_info(&trace_area);
    if (trace_area.is_storing)
    {
        return RETVAL(E_BUSY);
    }

    return trace_area_read_log(buf);
}

void trace_rom_printf(uint32_t attr, const char *fmt, ...)
{
    int32_t len;
    char buf[100];
    va_list ap;

    if (false == trace_area_store_is_enabled())
    {
        return;
    }

    va_start(ap, fmt);
    len = trace_format_buf(attr, buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (len)
    {
#ifdef TRACE_AUTO_FLUSH
        trace_auto_flush_timer_reset();
#endif
        if(RETVAL(E_OK) == sys_mutex_lock(log_append_mutex_id, SYS_WAIT_FOREVER))
        {
            trace_buffer_append(buf, (uint16_t)len);
            sys_mutex_unlock(log_append_mutex_id);
        }
    }
}

void trace_uart_printf(uint32_t attr, const char *fmt, ...)
{
    char *buf;
    uint8_t level;
    va_list ap;

    if (!trace_uart_print_is_enabled())
    {
        return;
    }

    level = GET_BITFIELD(attr, LOG_ATTR_LEVEL);
    if (level > LOG_PRINT_LEVEL)
    {
        return;
    }

    buf = mem_malloc(100);
    if (buf == NULL)
    {
        return;
    }
    memset(buf, 0, 100);
    va_start(ap, fmt);
    vsnprintf((char *)buf, 100, fmt, ap);
    buf[100-1] = '\0';
    va_end(ap);

    sys_enter_critical();
    bsp_uart_transmit(BSP_UART0, (uint8_t*)buf, strlen(buf), 0xFFFFFFFF);
    sys_exit_critical();
    mem_free(buf);
}

static void trace_init(void)
{
    memset(&g_trace, 0, sizeof(g_trace));
    trace_area_init();
    trace_area_store_register_callback(trace_store_complete);
    trace_stamp_timer_init();
#ifdef TRACE_AUTO_FLUSH
    trace_auto_flush_timer_init();
#endif

    log_append_mutex_id = sys_mutex_create(SYS_MUTEX(log_append_mutex));
}

BASIC_INITCALL("trace_core", trace_init);

static int32_t trace_get_info(char* argv[], int argc)
{
    uint16_t buf_ofs;
    uint8_t buf_idx;

    char newline[] = "\r\n";
    char str_head_page[10] = "H:";
    char str_tail_page[10] = "T:";
    char str_page_write_pos[10] = "Pos:";
    char str_write_buf_ofs[10] = "WOs:";
    char str_write_buf_idx[10] = "WIx:";

    char ascii_head_page[10];
    char ascii_tail_page[10];
    char ascii_tail_page_ofs[10];
    char ascii_write_buf_ofs[10];
    char ascii_write_buf_idx[10];

    trace_area_get_info(&trace_area_tmp);
    buf_ofs = trace_get_write_buf_ofs();
    buf_idx = trace_get_write_buf_idx();

    itoa(trace_area_tmp.head_page_idx, ascii_head_page, 10);
    itoa(trace_area_tmp.tail_page_idx, ascii_tail_page, 10);
    itoa(trace_area_tmp.page_write_pos, ascii_tail_page_ofs, 10);
    itoa(buf_ofs, ascii_write_buf_ofs, 10);
    itoa(buf_idx, ascii_write_buf_idx, 10);

    strcat(str_head_page, ascii_head_page);
    strcat(str_head_page, newline);
    strcat(str_tail_page, ascii_tail_page);
    strcat(str_tail_page, newline);
    strcat(str_page_write_pos, ascii_tail_page_ofs);
    strcat(str_page_write_pos, newline);
    strcat(str_write_buf_ofs, ascii_write_buf_ofs);
    strcat(str_write_buf_ofs, newline);
    strcat(str_write_buf_idx, ascii_write_buf_idx);
    strcat(str_write_buf_idx, newline);

    strcat(trace_info, str_head_page);
    strcat(trace_info, str_tail_page);
    strcat(trace_info, str_page_write_pos);
    strcat(trace_info, str_write_buf_ofs);
    strcat(trace_info, str_write_buf_idx);

    atcmd_send_response(AT_OK, (char*)&trace_info, strlen(trace_info));
    memset(trace_info, 0, sizeof(trace_info));

    return RETVAL(E_OK);
}

ATCMD_INIT("AT^RDLOGINFO?", 0, trace_get_info);

int32_t trace_cmd(char* argv[], int argc)
{
    if (argc != 1)
    {
        atcmd_send_response(AT_ERROR, AT_STR_ERR_ARGC, AT_STR_LEN(AT_STR_ERR_ARGC));
        return RETVAL(E_INVAL_LEN);
    }

    if (strncmp(argv[0], "0", strlen("0")) == 0)
    {
        trace_area_store_disable();
    }
    else if (strncmp(argv[0], "1", strlen("1")) == 0)
    {
        trace_area_store_enable();
    }
    else if (strncmp(argv[0], "2", strlen("2")) == 0)
    {
        trace_uart_print_toggle(true);
    }
    else if (strncmp(argv[0], "3", strlen("3")) == 0)
    {
        trace_uart_print_toggle(false);
    }

    atcmd_send_response(AT_OK, AT_STR_NONE, AT_STR_LEN(AT_STR_NONE));

    return RETVAL(E_OK);
}

ATCMD_INIT("AT^LOGCMD=", 0, trace_cmd);
