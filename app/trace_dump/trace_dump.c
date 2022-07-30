/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file trace_dump.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include "modules.h"
#include "dtcmd.h"
#include "errorno.h"
#include "trace.h"
#include "sys_cmsis.h"

#include "trace_dump.h"

sys_thread_id_t trace_dump_task_id;

typedef struct
{
    uint8_t step;
    int32_t result;
    uint16_t len;
    uint8_t pack_idx;
    char read_data[NVM_PAGE_SIZE];
    char post_data[TRACE_POST_DATA_LEN];
    char repost_data[TRACE_POST_DATA_LEN];
    uint16_t repost_len;
} trace_dump_t;

trace_dump_t trace_dump;

static void trace_dump_send(char* result, const char* str, uint16_t len)
{
    dtcmd_send_response(result, str, len);
}

static void trace_dump_post_data(void)
{
    trace_dump.len = trace_dump.result > TRACE_POST_DATA_LEN ? TRACE_POST_DATA_LEN : trace_dump.result;
    memcpy(trace_dump.post_data, trace_dump.read_data + trace_dump.pack_idx * TRACE_POST_DATA_LEN, trace_dump.len);
    trace_dump.repost_len = trace_dump.len;
    memcpy(trace_dump.repost_data, trace_dump.post_data, trace_dump.repost_len);
    trace_dump_send(DTCMD_RESP_DATA, trace_dump.post_data, trace_dump.len);
}

static void trace_dump_start(void)
{
    if (trace_dump.step == DUMP_STEP_STOP)
    {
        trace_dump.result = trace_dump_log(trace_dump.read_data);
        if (trace_dump.result == RETVAL(E_OK))
        {
            trace_dump_send(DT_OK, DUMP_STR_FINISH, DT_STR_LEN(DUMP_STR_FINISH));
            trace_stop_read();
            trace_store_enable();
        }
        else if (trace_dump.result > 0)
        {
            trace_dump.step = DUMP_STEP_POST_DATA;
            trace_dump_post_data();
        }
        else if (trace_dump.result == RETVAL(E_BUSY))
        {
            trace_dump_send(DT_OK, DUMP_STR_BUSY, DT_STR_LEN(DUMP_STR_BUSY));
        }
    }
    else
    {
        trace_dump_send(DT_OK, DUMP_STR_STARTED, strlen(DUMP_STR_STARTED));
    }
}

static void trace_remain_flush(void)
{
    if (trace_dump.step == DUMP_STEP_STOP)
    {
        trace_store_disable();
        trace_auto_flush_timer_set_ms(2);
        trace_auto_flush_remain_sts_set(AUTO_FLUSH_REMAIN_STS_IN_PROCESS);
        sys_thread_resume(trace_dump_task_id);
    }
    else
    {
        trace_dump_send(DT_OK, DUMP_STR_STARTED, strlen(DUMP_STR_STARTED));
    }
}

static void trace_dump_stop(void)
{
    if (trace_dump.step != DUMP_STEP_STOP)
    {
        trace_dump_send(DT_OK, DT_STR_NONE, DT_STR_LEN(DT_STR_NONE));
        trace_dump.step = DUMP_STEP_STOP;
        trace_stop_read();
        trace_dump.pack_idx = 0;
        trace_store_enable();
        sys_thread_suspend(trace_dump_task_id);
    }
    else if (trace_dump.step == DUMP_STEP_STOP)
    {
        trace_dump_send(DT_OK, DUMP_STR_STOPPED, DT_STR_LEN(DUMP_STR_STOPPED));
    }
}

static void trace_dump_next_pack(void)
{
    if (trace_dump.step == DUMP_STEP_STOP)
    {
        trace_dump_send(DT_OK, DUMP_STR_STOPPED, DT_STR_LEN(DUMP_STR_STOPPED));
    }
    else if (trace_dump.step == DUMP_STEP_POST_DATA)
    {
        trace_dump.pack_idx ++;
        trace_dump.result -= trace_dump.len;
        if (trace_dump.result == 0)
        {
            trace_dump.pack_idx = 0;
            trace_dump.result = trace_dump_log(trace_dump.read_data);
            if (trace_dump.result == RETVAL(E_OK))
            {
                trace_dump_send(DT_OK, DUMP_STR_FINISH, DT_STR_LEN(DUMP_STR_FINISH));
                trace_dump.step = DUMP_STEP_STOP;
                trace_stop_read();
                trace_store_enable();
            }
            else if (trace_dump.result > 0)
            {
                trace_dump.step = DUMP_STEP_POST_DATA;
                trace_dump_post_data();
            }
            return;
        }

        trace_dump_post_data();
    }
}

static void trace_dump_repost_pack(void)
{
    trace_dump_send(DTCMD_RESP_DATA, trace_dump.repost_data, trace_dump.repost_len);
}

static void trace_task_init(void)
{
    DTCMD_INIT("DT^DUMPLOG", trace_remain_flush);
    DTCMD_INIT("DT^DUMPNEXT", trace_dump_next_pack);
    DTCMD_INIT("DT^DUMPRESEND", trace_dump_repost_pack);
    DTCMD_INIT("DT^DUMPSTOP", trace_dump_stop);

    memset(&trace_dump, 0, sizeof(trace_dump));
}

APP_INITCALL("trace_task", trace_task_init);

static void trace_dump_task(void const *argument)
{
    while (1)
    {
        if (AUTO_FLUSH_REMAIN_STS_DONE == trace_auto_flush_remain_sts_get())
        {
            trace_dump_start();
            trace_auto_flush_remain_sts_set(AUTO_FLUSH_REMAIN_STS_STOP);
            trace_auto_flush_timer_restore();
        }
        sys_delay(10);
    }
}

SYS_THREAD_DEF(trace_dump_task, SYS_PRIORITY_ABOVE_NORMAL, 200);

static void trace_dump_task_init(void)
{
    trace_dump_task_id = sys_thread_create(SYS_THREAD(trace_dump_task), NULL);
    sys_thread_suspend(trace_dump_task_id);
}

APP_INITCALL("trace_dump_task", trace_dump_task_init);
