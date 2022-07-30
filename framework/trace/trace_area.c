/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file trace_area.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/
#include <stdarg.h>
#include <string.h>

#include "errorno.h"
#include "typedefs.h"
#include "modules.h"
#include "bsp_flash.h"
#include "sys_cmsis.h"
#include "atcmd.h"

#include "trace_area.h"

#define NVM_WRITE_WORDS(addr, buf, len)     bsp_flash_write_words(addr, buf, len)
#define NVM_READ_WORDS(addr, buf, len)      bsp_flash_read_words(addr, buf, len)
#define NVM_ERASE_PAGE(addr)                bsp_flash_erase_page(addr)

typedef struct {
    char *buff;
    uint8_t idx;
    uint16_t len;
} trace_store_msg_t;

typedef struct{
    uint8_t step;
} trace_read_t;

static bool g_trace_store_enable = true;
static bool g_trace_uart_print_enable = true;
static trace_read_t trace_read;

static trace_area_t g_trace_area;
static trace_area_store_cb_t trace_area_store_cb = NULL;

static void trace_store_task(void const *argument);
SYS_THREAD_DEF(trace_store_task, SYS_PRIORITY_ABOVE_NORMAL, 300);
SYS_MSG_QDEF(trace_store, 2, uint32_t);
sys_msg_id_t trace_store_msg_id;

void trace_uart_print_toggle(bool toggle)
{
    g_trace_uart_print_enable = toggle;
}

bool trace_uart_print_is_enabled(void)
{
    return g_trace_uart_print_enable;
}

void trace_area_store_enable(void)
{
    g_trace_store_enable = true;
}

void trace_area_store_disable(void)
{
    g_trace_store_enable = false;
}

bool trace_area_store_is_enabled(void)
{
    return g_trace_store_enable;
}

int32_t trace_area_store_register_callback(trace_area_store_cb_t cb)
{
    if (trace_area_store_cb != NULL)
    {
        return RETVAL(E_FAIL);
    }
    trace_area_store_cb = cb;
    
    return RETVAL(E_OK);
}

void trace_area_get_info(trace_area_t* trace_area_info_p)
{
    memcpy(trace_area_info_p, &g_trace_area, sizeof(trace_area_t));
}

static uint16_t trace_area_info_load_at_tail_page_pos(uint16_t tail_page_idx)
{
    uint32_t data_tmp;
    uint16_t i;

    for (i = 0; i < NVM_PAGE_SIZE; i += sizeof(uint32_t))
    {
        NVM_READ_WORDS(NVM_TRACE_BASE_ADDR + tail_page_idx * NVM_PAGE_SIZE + i, &data_tmp, 1);
        if (NVM_EMPTY_DATA == data_tmp)
        {
            return i;
        }
    }
    return i;
}

static void trace_area_load_info_at_tail_page_empty(void)
{
    uint32_t data_tmp;
    uint16_t page_index;

    NVM_READ_WORDS(NVM_TRACE_BASE_ADDR, &data_tmp, 1);
    //all empty
    if (NVM_EMPTY_DATA == data_tmp)
    {
        g_trace_area.head_page_idx = 0;
        g_trace_area.tail_page_idx = 0;
        g_trace_area.page_write_pos = 0;
    }
    else
    {   //some datas
        for (page_index = 0; page_index < NVM_TRACE_PAGE_NUM; page_index ++)
        {
            NVM_READ_WORDS(NVM_LAST_WORD_ADDR_IN_TRACE_PAGE(page_index), &data_tmp, 1);
            if (NVM_EMPTY_DATA == data_tmp)
            {
                g_trace_area.tail_page_idx = page_index;
                g_trace_area.head_page_idx = 0;
                g_trace_area.page_write_pos = trace_area_info_load_at_tail_page_pos(page_index);
                break;
            }
        }
    }
}

static void trace_area_load_info_at_tail_page_full(void)
{
    uint32_t data_tmp;
    uint16_t page_index;

    for (page_index = 0; page_index < NVM_TRACE_PAGE_NUM - 1; page_index ++)
    { 
        NVM_READ_WORDS(NVM_LAST_WORD_ADDR_IN_TRACE_PAGE(page_index) - sizeof(uint32_t), &data_tmp, 1);
        if (NVM_EMPTY_DATA == data_tmp)
        {
            g_trace_area.tail_page_idx = page_index;
            if (g_trace_area.tail_page_idx == NVM_TRACE_PAGE_NUM - 1)
            {
                g_trace_area.head_page_idx = 0;
            }
            else
            {
                g_trace_area.head_page_idx = g_trace_area.tail_page_idx + 1;
            }
            g_trace_area.page_write_pos = trace_area_info_load_at_tail_page_pos(page_index);
            break;
        }
    }
}

static void trace_area_find_page_empty_pos(void)
{
    uint32_t data_tmp;
    
    NVM_READ_WORDS(NVM_TRACE_LAST_WORD_ADDR, &data_tmp, 1);
    //not full
    if (NVM_EMPTY_DATA == data_tmp)
    {
        trace_area_load_info_at_tail_page_empty();
    }
    else
    {
        //full
        g_trace_area.is_critical = true;
        trace_area_load_info_at_tail_page_full();
    }
}

void trace_area_store_buf(char *buf, uint8_t idx, uint16_t len)
{
    uint16_t page_remain = NVM_PAGE_SIZE - g_trace_area.page_write_pos;
    uint32_t tail_page_ofs_addr = NVM_TRACE_BASE_ADDR + g_trace_area.tail_page_idx * NVM_PAGE_SIZE + g_trace_area.page_write_pos;

    sys_enter_critical();
    if (len < page_remain)
    {
        NVM_WRITE_WORDS(tail_page_ofs_addr, (uint32_t*)buf, len);
        g_trace_area.page_write_pos += len;
    }
    else if (len >= page_remain)
    {
        NVM_WRITE_WORDS(tail_page_ofs_addr, (uint32_t*)buf, page_remain);
        g_trace_area.tail_page_idx ++;
        if (g_trace_area.tail_page_idx >= NVM_TRACE_PAGE_NUM)
        {
            g_trace_area.tail_page_idx = 0;
            g_trace_area.is_critical = true;
        }
        NVM_ERASE_PAGE(NVM_TRACE_BASE_ADDR + g_trace_area.tail_page_idx * NVM_PAGE_SIZE);
        if (g_trace_area.is_critical == true)
        {
            g_trace_area.head_page_idx ++;
            if (g_trace_area.head_page_idx >= NVM_TRACE_PAGE_NUM)
            {
                g_trace_area.head_page_idx = 0;
            }
        }

        tail_page_ofs_addr = NVM_TRACE_BASE_ADDR + g_trace_area.tail_page_idx * NVM_PAGE_SIZE;
        NVM_WRITE_WORDS(tail_page_ofs_addr, (uint32_t*)(buf + page_remain), (len - page_remain));
        g_trace_area.page_write_pos = len - page_remain;
        page_remain = NVM_PAGE_SIZE - g_trace_area.page_write_pos;
    }
    (*trace_area_store_cb)(buf, idx, len);
    sys_exit_critical();
}

int32_t trace_read_page(char* buf)
{
    if (g_trace_area.read_page_index != g_trace_area.tail_page_idx)
    {
        NVM_READ_WORDS(NVM_TRACE_BASE_ADDR + g_trace_area.read_page_index * NVM_PAGE_SIZE, 
                      (uint32_t*)buf, NVM_PAGE_SIZE / 4);
        
        g_trace_area.read_page_index ++;
        if (g_trace_area.read_page_index >= NVM_TRACE_PAGE_NUM)
        {
            g_trace_area.read_page_index = 0;
        }
        return NVM_PAGE_SIZE;
    }
    else
    {
        return (RETVAL(E_OK));
    }
}

int32_t trace_read_tail_page(char* buf)
{
    if (g_trace_area.page_write_pos != 0)
    {
        NVM_READ_WORDS(NVM_TRACE_BASE_ADDR + g_trace_area.tail_page_idx * NVM_PAGE_SIZE,
                            (uint32_t*)buf, g_trace_area.page_write_pos / 4);

        return g_trace_area.page_write_pos;
    }

    return (RETVAL(E_OK));
}

void trace_area_stop_read(void)
{
    trace_read.step = READ_STS_STOP;
}

int32_t trace_area_read_log(char* buf)
{
    int32_t result = 0;

    if (trace_read.step == READ_STS_STOP)
    {
        g_trace_area.read_page_index = g_trace_area.head_page_idx;
        trace_read.step = READ_STS_IN_PROCESS;
        result = trace_read_page(buf);
        if (result > 0)
        {
            return result;
        }
        else if (result == 0)
        {
            result = trace_read_tail_page(buf);
            trace_read.step = READ_STS_FINISH;
            return result;
        }
    }
    else if (trace_read.step == READ_STS_IN_PROCESS)
    {
        result = trace_read_page(buf);
        if (result > 0)
        {
            return result;
        }
        else if (result == 0)
        {
            result = trace_read_tail_page(buf);
            trace_read.step = READ_STS_FINISH;
            return result;
        }
    }
    else if (trace_read.step == READ_STS_FINISH)
    {
        return RETVAL(E_OK);
    }
    return RETVAL(E_OK);
}

static void trace_store_task(void const *argument)
{
    sys_event_t evt;
    trace_store_msg_t *p_msg;

    while (1)
    {
        evt = sys_msg_get(trace_store_msg_id, SYS_WAIT_FOREVER);

        if (evt.status == SYS_EVENT_MESSAGE)
        {
            p_msg = (trace_store_msg_t*)evt.value.v;
        }
        else
        {
            continue;
        }

        g_trace_area.is_storing = true;
        trace_area_store_buf(p_msg->buff, p_msg->idx, p_msg->len);
        g_trace_area.is_storing = false;

        mem_free(p_msg);
    }
}

void trace_area_store_buffer(char *buf, uint8_t idx, uint16_t len)
{
    trace_store_msg_t *p_msg;

    p_msg = mem_malloc(sizeof(trace_store_msg_t));

    if (p_msg != NULL)
    {
        p_msg->buff = buf;
        p_msg->idx = idx;
        p_msg->len = len;

        int32_t ret;
        ret = sys_msg_put(trace_store_msg_id, (uint32_t)p_msg, 0);
        if(ret != RETVAL(E_OK))
        {
            mem_free(p_msg);
        }
    }
}

void trace_area_init(void)
{
    memset(&g_trace_area, 0, sizeof(g_trace_area));
    memset(&trace_read, 0 , sizeof(trace_read));
    trace_area_find_page_empty_pos();
}

static void trace_task_init(void)
{
    sys_thread_create(SYS_THREAD(trace_store_task), NULL);
    trace_store_msg_id = sys_msg_create(SYS_MSG_Q(trace_store), NULL);
}

APP_INITCALL("trace_area_store_task", trace_task_init);
