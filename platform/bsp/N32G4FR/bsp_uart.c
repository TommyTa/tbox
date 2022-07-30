/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ****************************************************************************
 * @file bsp_uart.c
 * @author tanxt
 * @version v1.0.0
 * ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "n32g4fr.h"
#include "stdarg.h"
#include "bsp_uart.h"
#include "sys_cmsis.h"

/****************************BSP_UART0 for comm*******************************/
#define N32_USART0                  USART1
#define N32_USART0_GPIO             GPIOA
#define N32_USART0_CLK              RCC_APB2_PERIPH_USART1
#define N32_USART0_GPIO_CLK         RCC_APB2_PERIPH_GPIOA
#define N32_USART0_RxPin            GPIO_PIN_10
#define N32_USART0_TxPin            GPIO_PIN_9
#define N32_USART0_APBxClkCmd       RCC_EnableAPB2PeriphClk
#define N32_USART0_DMAx             DMA1
#define N32_USART0_DMAx_CLK         RCC_AHB_PERIPH_DMA1
#define N32_USART0_DR_Base          (USART1_BASE + 0x04)
#define N32_USART0_Tx_DMA_Channel   DMA1_CH4
#define N32_USART0_Tx_DMA_FLAG      DMA1_FLAG_TC4
#define N32_USART0_DMA_TX_IRQn      DMA1_Channel4_IRQn
#define N32_USART0_IRQn             USART1_IRQn
#define N32_USART0_Rx_DMA_Channel   DMA1_CH5
#define N32_USART0_Rx_DMA_FLAG      DMA1_INT_TXC5
#define N32_USART0_DMA_RX_IRQn      DMA1_Channel5_IRQn

#define N32_UART0_BUF_SIZE          256

static uint8_t bsp_uart0_tx_buf[N32_UART0_BUF_SIZE];
static uint8_t bsp_uart0_rx_buf[N32_UART0_BUF_SIZE];

static bsp_uart_recv_cb_t bsp_uart_recv_cb = NULL;

uint8_t bsp_uart0_dma_rev_len = 0;
/*************************BSP_UART1 for debug printf**************************/
#define N32_USART1                  UART6
#define N32_USART1_GPIO             GPIOB
#define N32_USART1_CLK              RCC_APB2_PERIPH_UART6
#define N32_USART1_GPIO_CLK         RCC_APB2_PERIPH_GPIOB
#define N32_USART1_RxPin            GPIO_PIN_1
#define N32_USART1_TxPin            GPIO_PIN_0
#define N32_USART1_APBxClkCmd       RCC_EnableAPB2PeriphClk
#define N32_USART1_DMAx             DMA2
#define N32_USART1_DMAx_CLK         RCC_AHB_PERIPH_DMA2
#define N32_USART1_DR_Base          (UART6_BASE + 0x04)
#define N32_USART1_Tx_DMA_Channel   DMA2_CH2
#define N32_USART1_Tx_DMA_FLAG      DMA2_FLAG_TC2
#define N32_USART1_DMA_TX_IRQn      DMA2_Channel2_IRQn
#define N32_USART1_IRQn             UART6_IRQn
#define N32_USART1_Rx_DMA_Channel   DMA2_CH1
#define N32_USART1_Rx_DMA_FLAG      DMA2_INT_TXC1
#define N32_USART1_DMA_RX_IRQn      DMA2_Channel1_IRQn

#define N32_UART1_BUF_SIZE          256

static uint8_t bsp_uart1_tx_buf[N32_UART1_BUF_SIZE];
static uint8_t bsp_uart1_rx_buf[N32_UART1_BUF_SIZE];

static bsp_uart_recv_cb_t bsp_uart1_recv_cb = NULL;

uint8_t bsp_uart1_dma_rev_len = 0;
/*************************BSP_UART2 for debug printf**************************/
#define N32_USART2                  USART2
#define N32_USART2_GPIO             GPIOB
#define N32_USART2_CLK              RCC_APB1_PERIPH_USART2
#define N32_USART2_GPIO_CLK         RCC_APB2_PERIPH_GPIOB
#define N32_USART2_RxPin            GPIO_PIN_5
#define N32_USART2_TxPin            GPIO_PIN_4
#define N32_USART2_APBxClkCmd       RCC_EnableAPB1PeriphClk
#define N32_USART2_DMAx             DMA1
#define N32_USART2_DMAx_CLK         RCC_AHB_PERIPH_DMA1
#define N32_USART2_DR_Base          (USART2_BASE + 0x04)
#define N32_USART2_Tx_DMA_Channel   DMA1_CH7
#define N32_USART2_Tx_DMA_FLAG      DMA1_FLAG_TC7
#define N32_USART2_DMA_TX_IRQn      DMA1_Channel7_IRQn
#define N32_USART2_IRQn             USART2_IRQn
#define N32_USART2_Rx_DMA_Channel   DMA1_CH6
#define N32_USART2_Rx_DMA_FLAG      DMA1_INT_TXC6
#define N32_USART2_DMA_RX_IRQn      DMA1_Channel6_IRQn

#define N32_UART2_BUF_SIZE          256

static uint8_t bsp_uart2_tx_buf[N32_UART2_BUF_SIZE];
static uint8_t bsp_uart2_rx_buf[N32_UART2_BUF_SIZE];

static bsp_uart_recv_cb_t bsp_uart2_recv_cb = NULL;

uint8_t bsp_uart2_dma_rev_len = 0;
/**
 * @brief usart init
 * @param null
 * @retval null
 */
void bsp_uart_init(bsp_uart_t uart)
{
    if (uart == BSP_UART0)
    {
        memset(bsp_uart0_tx_buf, 0, N32_UART0_BUF_SIZE);
        memset(bsp_uart0_rx_buf, 0, N32_UART0_BUF_SIZE);
        RCC_EnableAHBPeriphClk(N32_USART0_DMAx_CLK, ENABLE);
        RCC_EnableAPB2PeriphClk(N32_USART0_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
        N32_USART0_APBxClkCmd(N32_USART0_CLK, ENABLE);

        GPIO_InitType GPIO_InitStructure;
#if !COMM_MODE_HALF_DUPLEX_USART0
        GPIO_InitStructure.Pin        = N32_USART0_RxPin;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
        GPIO_InitPeripheral(N32_USART0_GPIO, &GPIO_InitStructure);
#endif
        GPIO_InitStructure.Pin        = N32_USART0_TxPin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitPeripheral(N32_USART0_GPIO, &GPIO_InitStructure);

        USART_InitType USART_InitStructure;
        USART_InitStructure.BaudRate            = 115200;
        USART_InitStructure.WordLength          = USART_WL_8B;
        USART_InitStructure.StopBits            = USART_STPB_1;
        USART_InitStructure.Parity              = USART_PE_NO;
        USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
        USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
        USART_Init(N32_USART0, &USART_InitStructure);
#if COMM_MODE_HALF_DUPLEX_USART0
        USART_EnableHalfDuplex(N32_USART0, ENABLE);
#endif

        NVIC_InitType NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel                   = N32_USART0_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
#if COMM_MODE_HALF_DUPLEX_USART0
        NVIC_InitStructure.NVIC_IRQChannel                   = N32_USART0_DMA_TX_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
#endif
        DMA_InitType DMA_InitStructure;
        DMA_DeInit(N32_USART0_Tx_DMA_Channel);
        DMA_InitStructure.PeriphAddr     = N32_USART0_DR_Base;
        DMA_InitStructure.MemAddr        = (uint32_t)bsp_uart0_tx_buf;
        DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
        DMA_InitStructure.BufSize        = 0;
        DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
        DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
        DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
        DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
        DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
        DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
        DMA_Init(N32_USART0_Tx_DMA_Channel, &DMA_InitStructure);

        DMA_DeInit(N32_USART0_Rx_DMA_Channel);
        DMA_InitStructure.PeriphAddr = N32_USART0_DR_Base;
        DMA_InitStructure.MemAddr    = (uint32_t)bsp_uart0_rx_buf;
        DMA_InitStructure.Direction  = DMA_DIR_PERIPH_SRC;
        DMA_InitStructure.BufSize    = N32_UART0_BUF_SIZE;
        DMA_Init(N32_USART0_Rx_DMA_Channel, &DMA_InitStructure);

        USART_EnableDMA(N32_USART0, USART_DMAREQ_TX | USART_DMAREQ_RX, ENABLE);
        USART_ClrFlag(N32_USART0, USART_INT_IDLEF);
        USART_ConfigInt(N32_USART0, USART_INT_IDLEF, ENABLE);
#if COMM_MODE_HALF_DUPLEX_USART0
        DMA_ClearFlag(N32_USART0_Tx_DMA_FLAG, N32_USART0_DMAx);
        DMA_ConfigInt(N32_USART0_Tx_DMA_Channel, DMA_INT_TXC, ENABLE);
#endif
        DMA_EnableChannel(N32_USART0_Tx_DMA_Channel, ENABLE);
        DMA_EnableChannel(N32_USART0_Rx_DMA_Channel, ENABLE);
        USART_Enable(N32_USART0, ENABLE);
    }
    else if (uart == BSP_UART1)
    {
        memset(bsp_uart1_tx_buf, 0, N32_UART1_BUF_SIZE);
        memset(bsp_uart1_rx_buf, 0, N32_UART1_BUF_SIZE);
        RCC_EnableAHBPeriphClk(N32_USART1_DMAx_CLK, ENABLE);
        RCC_EnableAPB2PeriphClk(N32_USART1_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
        N32_USART1_APBxClkCmd(N32_USART1_CLK, ENABLE);

        GPIO_ConfigPinRemap(GPIO_RMP3_UART6, ENABLE);

        GPIO_InitType GPIO_InitStructure;
#if !COMM_MODE_HALF_DUPLEX_USART1
        GPIO_InitStructure.Pin        = N32_USART1_RxPin;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
        GPIO_InitPeripheral(N32_USART1_GPIO, &GPIO_InitStructure);
#endif
        GPIO_InitStructure.Pin        = N32_USART1_TxPin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
        GPIO_InitPeripheral(N32_USART1_GPIO, &GPIO_InitStructure);

        USART_InitType USART_InitStructure;
        USART_InitStructure.BaudRate            = 230400;
        USART_InitStructure.WordLength          = USART_WL_8B;
        USART_InitStructure.StopBits            = USART_STPB_1;
        USART_InitStructure.Parity              = USART_PE_NO;
        USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
        USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
        USART_Init(N32_USART1, &USART_InitStructure);
#if COMM_MODE_HALF_DUPLEX_USART1
        USART_EnableHalfDuplex(N32_USART1, ENABLE);
#endif
        NVIC_InitType NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel                   = N32_USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
#if COMM_MODE_HALF_DUPLEX_USART1
        NVIC_InitStructure.NVIC_IRQChannel                   = N32_USART1_DMA_TX_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
#endif
        DMA_InitType DMA_InitStructure;
        DMA_DeInit(N32_USART1_Tx_DMA_Channel);
        DMA_InitStructure.PeriphAddr     = N32_USART1_DR_Base;
        DMA_InitStructure.MemAddr        = (uint32_t)bsp_uart1_tx_buf;
        DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
        DMA_InitStructure.BufSize        = 0;
        DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
        DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
        DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
        DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
        DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
        DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
        DMA_Init(N32_USART1_Tx_DMA_Channel, &DMA_InitStructure);

        DMA_DeInit(N32_USART1_Rx_DMA_Channel);
        DMA_InitStructure.PeriphAddr = N32_USART1_DR_Base;
        DMA_InitStructure.MemAddr    = (uint32_t)bsp_uart1_rx_buf;
        DMA_InitStructure.Direction  = DMA_DIR_PERIPH_SRC;
        DMA_InitStructure.BufSize    = N32_UART1_BUF_SIZE;
        DMA_Init(N32_USART1_Rx_DMA_Channel, &DMA_InitStructure);

        USART_EnableDMA(N32_USART1, USART_DMAREQ_TX | USART_DMAREQ_RX, ENABLE);
        USART_ClrFlag(N32_USART1, USART_INT_IDLEF);
        USART_ConfigInt(N32_USART1, USART_INT_IDLEF, ENABLE);
#if COMM_MODE_HALF_DUPLEX_USART1
        DMA_ClearFlag(N32_USART1_Tx_DMA_FLAG, N32_USART1_DMAx);
        DMA_ConfigInt(N32_USART1_Tx_DMA_Channel, DMA_INT_TXC, ENABLE);
#endif
        DMA_EnableChannel(N32_USART1_Tx_DMA_Channel, ENABLE);
        DMA_EnableChannel(N32_USART1_Rx_DMA_Channel, ENABLE);
        USART_Enable(N32_USART1, ENABLE);
    }
    else if (uart == BSP_UART2)
    {
        memset(bsp_uart2_tx_buf, 0, N32_UART2_BUF_SIZE);
        memset(bsp_uart2_rx_buf, 0, N32_UART2_BUF_SIZE);
        RCC_EnableAHBPeriphClk(N32_USART2_DMAx_CLK, ENABLE);
        RCC_EnableAPB2PeriphClk(N32_USART2_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
        N32_USART2_APBxClkCmd(N32_USART2_CLK, ENABLE);

        GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_DISABLE, ENABLE);
        GPIO_ConfigPinRemap(GPIO_RMP3_USART2, ENABLE);

        GPIO_InitType GPIO_InitStructure;
#if !COMM_MODE_HALF_DUPLEX_USART2
        GPIO_InitStructure.Pin        = N32_USART2_RxPin;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
        GPIO_InitPeripheral(N32_USART2_GPIO, &GPIO_InitStructure);
#endif
        GPIO_InitStructure.Pin        = N32_USART2_TxPin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
        GPIO_InitPeripheral(N32_USART2_GPIO, &GPIO_InitStructure);

        USART_InitType USART_InitStructure;
        USART_InitStructure.BaudRate            = 230400;
        USART_InitStructure.WordLength          = USART_WL_8B;
        USART_InitStructure.StopBits            = USART_STPB_1;
        USART_InitStructure.Parity              = USART_PE_NO;
        USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
        USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
        USART_Init(N32_USART2, &USART_InitStructure);
#if COMM_MODE_HALF_DUPLEX_USART2
        USART_EnableHalfDuplex(N32_USART2, ENABLE);
#endif

        NVIC_InitType NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel                   = N32_USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
#if COMM_MODE_HALF_DUPLEX_USART2
        NVIC_InitStructure.NVIC_IRQChannel                   = N32_USART2_DMA_TX_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
#endif
        DMA_InitType DMA_InitStructure;
        DMA_DeInit(N32_USART2_Tx_DMA_Channel);
        DMA_InitStructure.PeriphAddr     = N32_USART2_DR_Base;
        DMA_InitStructure.MemAddr        = (uint32_t)bsp_uart2_tx_buf;
        DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
        DMA_InitStructure.BufSize        = 0;
        DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
        DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
        DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
        DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
        DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
        DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
        DMA_Init(N32_USART2_Tx_DMA_Channel, &DMA_InitStructure);

        DMA_DeInit(N32_USART2_Rx_DMA_Channel);
        DMA_InitStructure.PeriphAddr = N32_USART2_DR_Base;
        DMA_InitStructure.MemAddr    = (uint32_t)bsp_uart2_rx_buf;
        DMA_InitStructure.Direction  = DMA_DIR_PERIPH_SRC;
        DMA_InitStructure.BufSize    = N32_UART2_BUF_SIZE;
        DMA_Init(N32_USART2_Rx_DMA_Channel, &DMA_InitStructure);

        USART_EnableDMA(N32_USART2, USART_DMAREQ_TX | USART_DMAREQ_RX, ENABLE);
        USART_ClrFlag(N32_USART2, USART_INT_IDLEF);
        USART_ConfigInt(N32_USART2, USART_INT_IDLEF, ENABLE);
#if COMM_MODE_HALF_DUPLEX_USART2
        DMA_ClearFlag(N32_USART2_Tx_DMA_FLAG, N32_USART2_DMAx);
        DMA_ConfigInt(N32_USART2_Tx_DMA_Channel, DMA_INT_TXC, ENABLE);
#endif
        DMA_EnableChannel(N32_USART2_Tx_DMA_Channel, ENABLE);
        DMA_EnableChannel(N32_USART2_Rx_DMA_Channel, ENABLE);
        USART_Enable(N32_USART2, ENABLE);
    }
}

/**
 * @brief communication usart send data
 * @param data
 * @param len
 * @retval 1:too more data
 *         2:dma channel is busy
 */
uint32_t bsp_uart_transmit(bsp_uart_t uart, uint8_t* data, uint16_t len, uint32_t timeout)
{
    uint32_t ret = 0;

    if (uart == BSP_UART0)
    {
        //Wait for the last transfer to complete
        while (USART_GetFlagStatus(N32_USART0, USART_FLAG_TXC) != SET)
        {
            if (timeout == 0)
            {
                break;
            }
            timeout--;
        };
        DMA_EnableChannel(N32_USART0_Tx_DMA_Channel, DISABLE);
        if (0 == DMA_GetCurrDataCounter(N32_USART0_Tx_DMA_Channel))
        {
            if (len > N32_UART0_BUF_SIZE)
            {
                ret = 1;
            }
            else
            {
                memcpy(bsp_uart0_tx_buf, data, len);
                // trigger send
                DMA_SetCurrDataCounter(N32_USART0_Tx_DMA_Channel, len);
            }
        }
        else
        {
            ret = 2;
        }
#if COMM_MODE_HALF_DUPLEX_USART0
        USART_ClrFlag(N32_USART0, USART_INT_IDLEF);
        USART_GetIntStatus(N32_USART0, USART_INT_IDLEF);
        USART_ReceiveData(N32_USART0);
        USART_ConfigInt(N32_USART0, USART_INT_IDLEF, DISABLE);
        DMA_ClearFlag(N32_USART0_Tx_DMA_FLAG, N32_USART0_DMAx);
        DMA_ClearFlag(N32_USART0_Rx_DMA_FLAG, N32_USART0_DMAx);
        DMA_ConfigInt(N32_USART0_Tx_DMA_Channel, DMA_INT_TXC, ENABLE);
#endif
        DMA_EnableChannel(N32_USART0_Tx_DMA_Channel, ENABLE);
    }
    else if (uart == BSP_UART1)
    {
        while (USART_GetFlagStatus(N32_USART1, USART_FLAG_TXC) != SET)
        {
            if (timeout == 0)
            {
                break;
            }
            timeout--;
        }
        DMA_EnableChannel(N32_USART1_Tx_DMA_Channel, DISABLE);
        if (0 == DMA_GetCurrDataCounter(N32_USART1_Tx_DMA_Channel))
        {
            if (len > N32_UART1_BUF_SIZE)
            {
                ret = 1;
            }
            else
            {
                memcpy(bsp_uart1_tx_buf, data, len);
                // trigger send
                DMA_SetCurrDataCounter(N32_USART1_Tx_DMA_Channel, len);
            }
        }
        else
        {
            ret = 2;
        }
#if COMM_MODE_HALF_DUPLEX_USART1
        USART_ClrFlag(N32_USART1, USART_INT_IDLEF);
        USART_GetIntStatus(N32_USART1, USART_INT_IDLEF);
        USART_ReceiveData(N32_USART1);
        USART_ConfigInt(N32_USART1, USART_INT_IDLEF, DISABLE);
        DMA_ClearFlag(N32_USART1_Tx_DMA_FLAG, N32_USART1_DMAx);
        DMA_ClearFlag(N32_USART1_Rx_DMA_FLAG, N32_USART1_DMAx);
        DMA_ConfigInt(N32_USART1_Tx_DMA_Channel, DMA_INT_TXC, ENABLE);
#endif
        DMA_EnableChannel(N32_USART1_Tx_DMA_Channel, ENABLE);
    }
    else if (uart == BSP_UART2)
    {
        while (USART_GetFlagStatus(N32_USART2, USART_FLAG_TXC) != SET)
        {
            if (timeout == 0)
            {
                break;
            }
            timeout--;
        }
        DMA_EnableChannel(N32_USART2_Tx_DMA_Channel, DISABLE);
        if (0 == DMA_GetCurrDataCounter(N32_USART2_Tx_DMA_Channel))
        {
            if (len > N32_UART2_BUF_SIZE)
            {
                ret = 1;
            }
            else
            {
                memcpy(bsp_uart2_tx_buf, data, len);
                // trigger send
                DMA_SetCurrDataCounter(N32_USART2_Tx_DMA_Channel, len);
            }
        }
        else
        {
            ret = 2;
        }
#if COMM_MODE_HALF_DUPLEX_USART2
        USART_ClrFlag(N32_USART2, USART_INT_IDLEF);
        USART_GetIntStatus(N32_USART2, USART_INT_IDLEF);
        USART_ReceiveData(N32_USART2);
        USART_ConfigInt(N32_USART2, USART_INT_IDLEF, DISABLE);
        DMA_ClearFlag(N32_USART2_Tx_DMA_FLAG, N32_USART2_DMAx);
        DMA_ClearFlag(N32_USART2_Rx_DMA_FLAG, N32_USART2_DMAx);
        DMA_ConfigInt(N32_USART2_Tx_DMA_Channel, DMA_INT_TXC, ENABLE);
#endif
        DMA_EnableChannel(N32_USART2_Tx_DMA_Channel, ENABLE);
    }
    return ret;
}

/**
 * @brief
 * @param
 * @retval
 */
uint32_t bsp_uart_register_recv_callback(bsp_uart_t uart, bsp_uart_recv_cb_t cb)
{
    if (uart == BSP_UART0)
    {
        if (bsp_uart_recv_cb != NULL)
        {
            return 1;
        }
        bsp_uart_recv_cb = cb;
    }
    else if (uart == BSP_UART1)
    {
        if (bsp_uart1_recv_cb != NULL)
        {
            return 1;
        }
        bsp_uart1_recv_cb = cb;
    }
    else if (uart == BSP_UART2)
    {
        if (bsp_uart2_recv_cb != NULL)
        {
            return 1;
        }
        bsp_uart2_recv_cb = cb;
    }
    return 0;
}

/**
 * @brief
 * @param
 * @retval
 */
void bsp_uart_irq_handler(bsp_uart_t uart)
{
    if (uart == BSP_UART0)
    {
        if (USART_GetIntStatus(N32_USART0, USART_INT_IDLEF) != RESET)
        {
            //idle flag bit is read only in status register,read status register and read data register to clear
            USART_ReceiveData(N32_USART0);

            DMA_EnableChannel(N32_USART0_Rx_DMA_Channel, DISABLE);
            bsp_uart0_dma_rev_len = DMA_GetCurrDataCounter(N32_USART0_Rx_DMA_Channel);
            sys_enter_critical();
            (*bsp_uart_recv_cb)(uart, (uint8_t *)bsp_uart0_rx_buf, N32_UART0_BUF_SIZE - bsp_uart0_dma_rev_len);
            sys_exit_critical();
            DMA_SetCurrDataCounter(N32_USART0_Rx_DMA_Channel, N32_UART0_BUF_SIZE);
            if (DMA_GetFlagStatus(N32_USART0_Rx_DMA_FLAG, N32_USART0_DMAx) != RESET)
            {
                DMA_ClearFlag(N32_USART0_Rx_DMA_FLAG, N32_USART0_DMAx);
            }
            DMA_EnableChannel(N32_USART0_Rx_DMA_Channel, ENABLE);
        }
        else if(USART_GetIntStatus(N32_USART0, USART_INT_OREF) != RESET)
        {
            USART_ReceiveData(N32_USART0);
        }
        else if(USART_GetIntStatus(N32_USART0, USART_INT_ERRF) != RESET)
        {
            USART_ReceiveData(N32_USART0);
        }
        else if(USART_GetIntStatus(N32_USART0, USART_INT_NEF) != RESET)
        {
            USART_ReceiveData(N32_USART0);
        }
        else if(USART_GetIntStatus(N32_USART0, USART_INT_FEF) != RESET)
        {
            USART_ReceiveData(N32_USART0);
        }
        else if(USART_GetIntStatus(N32_USART0, USART_INT_PEF) != RESET)
        {
            USART_ReceiveData(N32_USART0);
        }
    }
    else if (uart == BSP_UART1)
    {
        if (USART_GetIntStatus(N32_USART1, USART_INT_IDLEF) != RESET)
        {
            USART_ReceiveData(N32_USART1);

            DMA_EnableChannel(N32_USART1_Rx_DMA_Channel, DISABLE);
            bsp_uart1_dma_rev_len = DMA_GetCurrDataCounter(N32_USART1_Rx_DMA_Channel);
            sys_enter_critical();
            (*bsp_uart1_recv_cb)(uart, (uint8_t *)bsp_uart1_rx_buf, N32_UART1_BUF_SIZE - bsp_uart1_dma_rev_len);
            sys_exit_critical();
            DMA_SetCurrDataCounter(N32_USART1_Rx_DMA_Channel, N32_UART1_BUF_SIZE);
            if (DMA_GetFlagStatus(N32_USART1_Rx_DMA_FLAG, N32_USART1_DMAx) != RESET)
            {
                DMA_ClearFlag(N32_USART1_Rx_DMA_FLAG, N32_USART1_DMAx);
            }
            DMA_EnableChannel(N32_USART1_Rx_DMA_Channel, ENABLE);
        }
        else if(USART_GetIntStatus(N32_USART1, USART_INT_OREF) != RESET)
        {
            USART_ReceiveData(N32_USART1);
        }
        else if(USART_GetIntStatus(N32_USART1, USART_INT_ERRF) != RESET)
        {
            USART_ReceiveData(N32_USART1);
        }
        else if(USART_GetIntStatus(N32_USART1, USART_INT_NEF) != RESET)
        {
            USART_ReceiveData(N32_USART1);
        }
        else if(USART_GetIntStatus(N32_USART1, USART_INT_FEF) != RESET)
        {
            USART_ReceiveData(N32_USART1);
        }
        else if(USART_GetIntStatus(N32_USART1, USART_INT_PEF) != RESET)
        {
            USART_ReceiveData(N32_USART1);
        }
    }
    else if (uart == BSP_UART2)
    {
        if (USART_GetIntStatus(N32_USART2, USART_INT_IDLEF) != RESET)
        {
            USART_ReceiveData(N32_USART2);

            DMA_EnableChannel(N32_USART2_Rx_DMA_Channel, DISABLE);
            bsp_uart2_dma_rev_len = DMA_GetCurrDataCounter(N32_USART2_Rx_DMA_Channel);
            sys_enter_critical();
            (*bsp_uart2_recv_cb)(uart, (uint8_t *)bsp_uart2_rx_buf, N32_UART2_BUF_SIZE - bsp_uart2_dma_rev_len);
            sys_exit_critical();
            DMA_SetCurrDataCounter(N32_USART2_Rx_DMA_Channel, N32_UART2_BUF_SIZE);
            if (DMA_GetFlagStatus(N32_USART2_Rx_DMA_FLAG, N32_USART2_DMAx) != RESET)
            {
                DMA_ClearFlag(N32_USART2_Rx_DMA_FLAG, N32_USART2_DMAx);
            }
            DMA_EnableChannel(N32_USART2_Rx_DMA_Channel, ENABLE);
        }
        else if(USART_GetIntStatus(N32_USART2, USART_INT_OREF) != RESET)
        {
            USART_ReceiveData(N32_USART2);
        }
        else if(USART_GetIntStatus(N32_USART2, USART_INT_ERRF) != RESET)
        {
            USART_ReceiveData(N32_USART2);
        }
        else if(USART_GetIntStatus(N32_USART2, USART_INT_NEF) != RESET)
        {
            USART_ReceiveData(N32_USART2);
        }
        else if(USART_GetIntStatus(N32_USART2, USART_INT_FEF) != RESET)
        {
            USART_ReceiveData(N32_USART2);
        }
        else if(USART_GetIntStatus(N32_USART2, USART_INT_PEF) != RESET)
        {
            USART_ReceiveData(N32_USART2);
        }
    }
}

void bsp_uart_dma_tx_irq_handler(bsp_uart_t uart)
{
    if (uart == BSP_UART0)
    {
#if COMM_MODE_HALF_DUPLEX_USART0
        if (DMA_GetIntStatus(N32_USART0_Tx_DMA_FLAG, N32_USART0_DMAx) != RESET)
        {
            DMA_ClearFlag(N32_USART0_Tx_DMA_FLAG, N32_USART0_DMAx);
            DMA_ConfigInt(N32_USART0_Tx_DMA_Channel, DMA_INT_TXC, DISABLE);
            DMA_SetCurrDataCounter(N32_USART0_Rx_DMA_Channel, N32_UART0_BUF_SIZE);
            if (DMA_GetFlagStatus(N32_USART0_Rx_DMA_FLAG, N32_USART0_DMAx) != RESET)
            {
                DMA_ClearFlag(N32_USART0_Rx_DMA_FLAG, N32_USART0_DMAx);
            }
            memset(bsp_uart0_rx_buf, 0, N32_UART0_BUF_SIZE);
            USART_ClrFlag(N32_USART0, USART_INT_IDLEF);
            USART_GetIntStatus(N32_USART0, USART_INT_IDLEF);
            USART_ReceiveData(N32_USART0);
            USART_ConfigInt(N32_USART0, USART_INT_IDLEF, ENABLE);
        }
#endif
    }
    else if (uart == BSP_UART1)
    {
#if COMM_MODE_HALF_DUPLEX_USART1
        if (DMA_GetIntStatus(N32_USART1_Tx_DMA_FLAG, N32_USART1_DMAx) != RESET)
        {
            DMA_ClearFlag(N32_USART1_Tx_DMA_FLAG, N32_USART1_DMAx);
            DMA_ConfigInt(N32_USART1_Tx_DMA_Channel, DMA_INT_TXC, DISABLE);
            DMA_SetCurrDataCounter(N32_USART1_Rx_DMA_Channel, N32_UART1_BUF_SIZE);
            if (DMA_GetFlagStatus(N32_USART1_Rx_DMA_FLAG, N32_USART1_DMAx) != RESET)
            {
                DMA_ClearFlag(N32_USART1_Rx_DMA_FLAG, N32_USART1_DMAx);
            }
            memset(bsp_uart1_rx_buf, 0, N32_UART1_BUF_SIZE);
            USART_ClrFlag(N32_USART1, USART_INT_IDLEF);
            USART_GetIntStatus(N32_USART1, USART_INT_IDLEF);
            USART_ReceiveData(N32_USART1);
            USART_ConfigInt(N32_USART1, USART_INT_IDLEF, ENABLE);
        }
#endif
    }
    else if (uart == BSP_UART2)
    {
#if COMM_MODE_HALF_DUPLEX_USART2
        if (DMA_GetIntStatus(N32_USART2_Tx_DMA_FLAG, N32_USART2_DMAx) != RESET)
        {
            DMA_ClearFlag(N32_USART2_Tx_DMA_FLAG, N32_USART2_DMAx);
            DMA_ConfigInt(N32_USART2_Tx_DMA_Channel, DMA_INT_TXC, DISABLE);
            DMA_SetCurrDataCounter(N32_USART2_Rx_DMA_Channel, N32_UART2_BUF_SIZE);
            if (DMA_GetFlagStatus(N32_USART2_Rx_DMA_FLAG, N32_USART2_DMAx) != RESET)
            {
                DMA_ClearFlag(N32_USART2_Rx_DMA_FLAG, N32_USART2_DMAx);
            }
            memset(bsp_uart2_rx_buf, 0, N32_UART2_BUF_SIZE);
            USART_ClrFlag(N32_USART2, USART_INT_IDLEF);
            USART_GetIntStatus(N32_USART2, USART_INT_IDLEF);
            USART_ReceiveData(N32_USART2);
            USART_ConfigInt(N32_USART2, USART_INT_IDLEF, ENABLE);
        }
#endif
    }
}
