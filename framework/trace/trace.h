/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file trace.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/
#ifndef _TRACE_H_
#define _TRACE_H_

#include "typedefs.h"

#define LOG_PRINT_LEVEL 2

typedef enum {
    LOG_LEVEL_CRITICAL  = 0,
    LOG_LEVEL_ERROR     = 1,
    LOG_LEVEL_WARN      = 2,
    LOG_LEVEL_NOTIFY    = 3,
    LOG_LEVEL_INFO      = 4,
    LOG_LEVEL_DEBUG     = 5,
    LOG_LEVEL_QTY,
} log_level_t;

// Log Attributes
#define LOG_ATTR_ARG_NUM_SHIFT              0
#define LOG_ATTR_ARG_NUM_MASK               (0xF << LOG_ATTR_ARG_NUM_SHIFT)
#define LOG_ATTR_ARG_NUM(n)                 BITFIELD_VAL(LOG_ATTR_ARG_NUM, n)
#define LOG_ATTR_LEVEL_SHIFT                4
#define LOG_ATTR_LEVEL_MASK                 (0x7 << LOG_ATTR_LEVEL_SHIFT)
#define LOG_ATTR_LEVEL(n)                   BITFIELD_VAL(LOG_ATTR_LEVEL, n)
#define LOG_ATTR_MOD_SHIFT                  7
#define LOG_ATTR_MOD_MASK                   (0x7F << LOG_ATTR_MOD_SHIFT)
#define LOG_ATTR_MOD(n)                     BITFIELD_VAL(LOG_ATTR_MOD, n)
#define LOG_ATTR_IMM                        (1 << 14)
#define LOG_ATTR_NO_LF                      (1 << 15)
#define LOG_ATTR_NO_TS                      (1 << 16)
#define LOG_ATTR_NO_ID                      (1 << 17)

// Count variadic argument number
#define _VAR_ARG_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, ...) a12
#define COUNT_ARG_NUM(...)                  _VAR_ARG_12(unused, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#if defined(TRACE_STR_SECTION)
#define CONCAT_(x, y)                       x##y
#define CONCATS(x, y)                       CONCAT_(x, y)
#define __trcname                           CONCATS(__trc, __LINE__)
#define TRC_STR_LOC                         __attribute__((section(TO_STRING(CONCATS(.trc_str, __LINE__)))))
#define TRC_STR(s)                          (({ static const char TRC_STR_LOC __trcname[] = (s); __trcname; }))
#else
#define TRC_STR_LOC
#define TRC_STR(s)                          (s)
#endif

#if 1
#define REL_LOG(attr, fmt, ...)             trace_rom_printf(((attr) & ~LOG_ATTR_ARG_NUM_MASK) | \
                                                            LOG_ATTR_ARG_NUM(COUNT_ARG_NUM(unused, ##__VA_ARGS__)), \
                                                            TRC_STR(fmt), ##__VA_ARGS__)
#else
#define REL_LOG(attr, fmt, ...)             trace_uart_printf(((attr) & ~LOG_ATTR_ARG_NUM_MASK) | \
                                                            LOG_ATTR_ARG_NUM(COUNT_ARG_NUM(unused, ##__VA_ARGS__)), \
                                                            fmt, ##__VA_ARGS__)
#endif

#define NORM_LOG                            REL_LOG

#define LOG_CRITICAL(attr, fmt, ...)        NORM_LOG(((attr) & ~LOG_ATTR_LEVEL_MASK) | LOG_ATTR_LEVEL(LOG_LEVEL_CRITICAL), \
                                                    fmt, ##__VA_ARGS__)
#define LOG_ERROR(attr, fmt, ...)           NORM_LOG(((attr) & ~LOG_ATTR_LEVEL_MASK) | LOG_ATTR_LEVEL(LOG_LEVEL_ERROR), \
                                                    fmt, ##__VA_ARGS__)
#define LOG_WARN(attr, fmt, ...)            NORM_LOG(((attr) & ~LOG_ATTR_LEVEL_MASK) | LOG_ATTR_LEVEL(LOG_LEVEL_WARN), \
                                                    fmt, ##__VA_ARGS__)
#define LOG_NOTIFY(attr, fmt, ...)          NORM_LOG(((attr) & ~LOG_ATTR_LEVEL_MASK) | LOG_ATTR_LEVEL(LOG_LEVEL_NOTIFY), \
                                                    fmt, ##__VA_ARGS__)
#define LOG_INFO(attr, fmt, ...)            NORM_LOG(((attr) & ~LOG_ATTR_LEVEL_MASK) | LOG_ATTR_LEVEL(LOG_LEVEL_INFO), \
                                                    fmt, ##__VA_ARGS__)
#define LOG_DEBUG(attr, fmt, ...)           NORM_LOG(((attr) & ~LOG_ATTR_LEVEL_MASK) | LOG_ATTR_LEVEL(LOG_LEVEL_DEBUG), \
                                                    fmt, ##__VA_ARGS__)

typedef void (*trace_store_cb_t)(void);

typedef enum
{
    AUTO_FLUSH_REMAIN_STS_STOP = 0,
    AUTO_FLUSH_REMAIN_STS_IN_PROCESS,
    AUTO_FLUSH_REMAIN_STS_DONE,
}auto_flush_remain_sts_t;

int32_t trace_dump_log(char* buf);
void trace_stop_read(void);
void trace_rom_printf(uint32_t attr, const char *fmt, ...);
void trace_uart_printf(uint32_t attr, const char *fmt, ...);
void trace_store_disable(void);
void trace_store_enable(void);
void trace_auto_flush_remain_sts_set(auto_flush_remain_sts_t sts);
auto_flush_remain_sts_t trace_auto_flush_remain_sts_get(void);
void trace_auto_flush_timer_set_ms(uint32_t time_ms);
void trace_auto_flush_timer_restore(void);

#endif // _TRACE_H_
