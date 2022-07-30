/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ****************************************************************************
 * @file bsp_spi.c
 * @author tanxt
 * @version v1.0.0
 * ***************************************************************************/
#include <stdio.h>

#include "n32g4fr.h"
#include "n32g4fr_gpio.h"
#include "modules.h"
#include "atcmd.h"
#include "errorno.h"
#include "x_stype.h"

#include "bsp_spi.h"

#define SPI_CS_GPIO_CLK                 RCC_APB2_PERIPH_GPIOA
#define SPI_MOSI_GPIO_CLK               RCC_APB2_PERIPH_GPIOA
#define SPI_MISO_GPIO_CLK               RCC_APB2_PERIPH_GPIOA
#define SPI_SCK_GPIO_CLK                RCC_APB2_PERIPH_GPIOA
#define SPI1_CLK                        RCC_APB2_PERIPH_SPI1

#define SPI_CS_PIN                      GPIO_PIN_4 
#define SPI_SCK_PIN                     GPIO_PIN_5 
#define SPI_MISO_PIN                    GPIO_PIN_6 
#define SPI_MOSI_PIN                    GPIO_PIN_7 

#define SPI_SCK_GPIO_PORT               GPIOA      
#define SPI_MISO_GPIO_PORT              GPIOA      
#define SPI_MOSI_GPIO_PORT              GPIOA      
#define SPI_CS_GPIO_PORT                GPIOA      

#define SPI1_BUS                        SPI1
#define SPI2_BUS                        SPI2
#define SPI3_BUS                        SPI3

#define TRUE                            1
#define FALSE                           0

typedef enum 
{
    SPI_CS_LOW = 0,
    SPI_CS_HIGH,
} spi_cs_level_t;


static void spi_rcc_cfg(bsp_spi_bus_t bus)
{
    if (BSP_SPI_BUS1 == bus) 
    {
        RCC_EnableAPB2PeriphClk(
            SPI_CS_GPIO_CLK | SPI_MOSI_GPIO_CLK | SPI_MISO_GPIO_CLK | SPI_SCK_GPIO_CLK | RCC_APB2_PERIPH_GPIOD, ENABLE);

        RCC_EnableAPB2PeriphClk(SPI1_CLK, ENABLE);
    }
}

static void spi_gpio_cfg(bsp_spi_bus_t bus)
{
    GPIO_InitType GPIO_InitStructure;

    if (BSP_SPI_BUS1 == bus) 
    {
        GPIO_InitStructure.Pin        = SPI_SCK_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitPeripheral(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.Pin = SPI_MOSI_PIN;
        GPIO_InitPeripheral(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.Pin       = SPI_MISO_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;         
        GPIO_InitPeripheral(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.Pin       = SPI_CS_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitPeripheral(SPI_CS_GPIO_PORT, &GPIO_InitStructure);
    }
}

static void bsp_spi_cs_level(bsp_spi_bus_t bus, spi_cs_level_t level)
{
    if (BSP_SPI_BUS1 == bus)
    {
        if (SPI_CS_LOW == level)
        {
            GPIO_ResetBits(GPIOA, SPI_CS_PIN);
        }
        else if (SPI_CS_HIGH == level)
        {
            GPIO_SetBits(GPIOA, SPI_CS_PIN);
        }
    }   
}

void bsp_spi_init(bsp_spi_bus_t bus)
{
    SPI_InitType spi_type;
    spi_type.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX,
    spi_type.SpiMode       = SPI_MODE_MASTER,
    spi_type.DataLen       = SPI_DATA_SIZE_8BITS,
    spi_type.CLKPOL        = SPI_CLKPOL_HIGH,
    spi_type.CLKPHA        = SPI_CLKPHA_SECOND_EDGE,
    spi_type.NSS           = SPI_NSS_SOFT,
    spi_type.BaudRatePres  = SPI_BR_PRESCALER_4,
    spi_type.FirstBit      = SPI_FB_MSB,
    spi_type.CRCPoly       = 7,

    spi_rcc_cfg(bus);
    spi_gpio_cfg(bus);
    bsp_spi_cs_level(bus, SPI_CS_HIGH);
    if (BSP_SPI_BUS1 == bus)
    {
        SPI_Init(SPI1, &spi_type);
        SPI_Enable(SPI1, ENABLE);
    }
}

int32_t bsp_spi_transmit_byte(bsp_spi_bus_t spi_bus, uint8_t data)
{
    return bsp_spi_transmit_bytes(spi_bus, &data, sizeof(uint8_t));   
}

int32_t bsp_spi_transmit_bytes(bsp_spi_bus_t spi_bus, uint8_t *tx_buf, int16_t len)
{
    int16_t time_out = 0;

    if ((NULL == tx_buf) || (len <= 0))
    {
        return RETVAL(E_NULL);
    }
        
    if (BSP_SPI_BUS1 == spi_bus) 
    {
        bsp_spi_cs_level(BSP_SPI_BUS1, SPI_CS_LOW);

        while (len--)
        {  
            while (SPI_I2S_GetStatus(SPI1_BUS, SPI_I2S_TE_FLAG) == RESET) 
            {
                time_out++;
                if (time_out > 200)
                {
                    return RETVAL(E_SEND);
                }    
            }
            SPI_I2S_TransmitData(SPI1_BUS, *tx_buf);
            time_out = 0;

            while (SPI_I2S_GetStatus(SPI1_BUS, SPI_I2S_RNE_FLAG) == RESET) 
            {
                time_out++;
                if (time_out > 200)
                {
                    return RETVAL(E_RECV);
                }            
            }
            SPI_I2S_ReceiveData(SPI1_BUS);

            tx_buf++;
            time_out = 0;
        }      

        bsp_spi_cs_level(BSP_SPI_BUS1, SPI_CS_HIGH);
    }
    return RETVAL(E_OK);  
}

int32_t bsp_spi_transfer(bsp_spi_bus_t spi_bus, uint8_t *tx_buf, int16_t len, uint8_t *rx_buf)
{
    int16_t time_out = 0;

    if ((NULL == tx_buf) || (NULL == rx_buf) || (len <= 0))
    {
        return RETVAL(E_NULL);
    }

    if (BSP_SPI_BUS1 == spi_bus) 
    {
        bsp_spi_cs_level(BSP_SPI_BUS1, SPI_CS_LOW);

        while (len--)
        {
            while (SPI_I2S_GetStatus(SPI1_BUS, SPI_I2S_TE_FLAG) == RESET) 
            {
                time_out++;
                if (time_out > 200)
                {
                    return RETVAL(E_SEND);
                }    
            }
            SPI_I2S_TransmitData(SPI1_BUS, *tx_buf);
            time_out = 0;

            while (SPI_I2S_GetStatus(SPI1_BUS, SPI_I2S_RNE_FLAG) == RESET) 
            {
                time_out++;
                if (time_out > 200)
                {
                    return RETVAL(E_RECV);
                }            
            }
            *rx_buf = SPI_I2S_ReceiveData(SPI1_BUS);

            tx_buf++;
            rx_buf++;
            time_out = 0;
        }

        bsp_spi_cs_level(BSP_SPI_BUS1, SPI_CS_HIGH);
    }
    return RETVAL(E_OK);  
}

