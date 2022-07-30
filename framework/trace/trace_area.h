/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file trace_area.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/
#ifndef _TRACE_AREA_H_
#define _TRACE_AREA_H_

#include "typedefs.h"
#include "partition.h"
#include "sys_cmsis.h"

#define NVM_PAGE_SIZE                               2048
#define NVM_TRACE_BASE_ADDR                         FLASH_OTA_REGION_BASE
#define NVM_TRACE_SIZE                              0x20000
#define NVM_TRACE_PAGE_NUM                          (NVM_TRACE_SIZE / NVM_PAGE_SIZE)

#define NVM_TRACE_LAST_WORD_ADDR                    (NVM_TRACE_BASE_ADDR + (NVM_TRACE_PAGE_NUM - 1) * NVM_PAGE_SIZE + NVM_PAGE_SIZE - sizeof(uint32_t))
#define NVM_LAST_WORD_ADDR_IN_TRACE_PAGE(page)      (NVM_TRACE_BASE_ADDR + page * NVM_PAGE_SIZE + NVM_PAGE_SIZE - sizeof(uint32_t))

#define NVM_EMPTY_DATA                              0xFFFFFFFF
#define NVM_SPECIAL_DATA                            0x13882710 /* 327690000 */

typedef enum
{
    READ_STS_STOP = 0,
    READ_STS_IN_PROCESS,
    READ_STS_FINISH,
} trace_read_status_t;

typedef struct {
    uint16_t head_page_idx;
    uint16_t tail_page_idx;
    uint16_t page_write_pos;
    uint16_t read_page_index;
    bool is_critical;
    bool is_storing;
} trace_area_t;

typedef void (*trace_area_store_cb_t)(char *buf, uint8_t idx, uint16_t len);

void trace_area_init(void);
int32_t trace_area_store_register_callback(trace_area_store_cb_t cb);
void trace_area_store_buffer(char *buf, uint8_t idx, uint16_t len);
int32_t trace_area_read_log(char* buf);
void trace_area_get_info(trace_area_t* trace_area_info_p);
void trace_area_stop_read(void);
void trace_area_store_enable(void);
void trace_area_store_disable(void);
bool trace_area_store_is_enabled(void);
void trace_uart_print_toggle(bool toggle);
bool trace_uart_print_is_enabled(void);

#endif //_TRACE_MGR_H_
