/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_usart.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "typedefs.h"

typedef enum {
    BSP_UART0,
    BSP_UART1,
    BSP_UART2,
} bsp_uart_t;

typedef void (*bsp_uart_recv_cb_t)(bsp_uart_t uart, uint8_t* data, uint16_t len);

void bsp_uart_init(bsp_uart_t uart);
uint32_t bsp_uart_transmit(bsp_uart_t uart, uint8_t* data, uint16_t len, uint32_t timeout);
uint32_t bsp_uart_register_recv_callback(bsp_uart_t uart, bsp_uart_recv_cb_t cb);
void bsp_uart_irq_handler(bsp_uart_t uart);
void bsp_uart_dma_tx_irq_handler(bsp_uart_t uart);

#endif // __BSP_USART_H__
