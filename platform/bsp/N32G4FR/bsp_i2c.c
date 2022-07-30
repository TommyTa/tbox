/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_i2c.c
 * @author tanxt
 * @reviser tanxt in 2021/3/11
 * @version v1.1.0
 * **************************************************************************/
#include <stdio.h>
#include "sys_cmsis.h"
#include "modules.h"
#include "errorno.h"
#include "n32g4fr_i2c.h"
#include "log.h"

#include "bsp_i2c.h"
#include "n32g4fr.h"

#define I2C_MODE_SOFTWARE
/******************************i2c1 gpio cfg*****************************/
#define BSP_I2C1            I2C1
#define BSP_I2C1_EV_IRQ     I2C1_EV_IRQn
#define BSP_I2C1_ER_IRQ     I2C1_ER_IRQn
#define BSP_I2C1_CLK        RCC_APB1_PERIPH_I2C1

#define I2C1_SCL_PIN        GPIO_PIN_6
#define I2C1_SDA_PIN        GPIO_PIN_5
#define I2C1_GPIO_PORT      GPIOB
#define I2C1_GPIO_PORT_CLK  RCC_APB2_PERIPH_GPIOB

#ifdef BSP_I2C_MULTI
/******************************i2c2 gpio cfg*****************************/
#define BSP_I2C2            I2C2
#define BSP_I2C2_EV_IRQ     I2C2_EV_IRQn
#define BSP_I2C2_ER_IRQ     I2C2_ER_IRQn
#define BSP_I2C2_CLK        RCC_APB1_PERIPH_I2C2

#define I2C2_SCL_PIN        GPIO_PIN_4
#define I2C2_SDA_PIN        GPIO_PIN_5
#define I2C2_GPIO_PORT      GPIOC
#define I2C2_GPIO_PORT_CLK  RCC_APB2_PERIPH_GPIOC

/******************************i2c3 gpio cfg*****************************/
#define BSP_I2C3            I2C3
#define BSP_I2C3_EV_IRQ     I2C3_EV_IRQn
#define BSP_I2C3_ER_IRQ     I2C3_ER_IRQn
#define BSP_I2C3_CLK        RCC_APB2_PERIPH_I2C3

#define I2C3_SCL_PIN        GPIO_PIN_10
#define I2C3_SDA_PIN        GPIO_PIN_11
#define I2C3_GPIO_PORT      GPIOB
#define I2C3_GPIO_PORT_CLK  RCC_APB2_PERIPH_GPIOB
#endif

#define I2C_TIMEOUT         ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT    ((uint32_t)(30 * I2C_TIMEOUT))
#define I2C_SPEED           100000

typedef struct {
    uint16_t scl;
    uint16_t sda;
    uint32_t speed;
    uint32_t mode;
} i2c_gpio_cfg_t;

typedef struct {
    I2C_Module* base;
    IRQn_Type irq;
    uint32_t clk;
    uint32_t gpiox;
    i2c_gpio_cfg_t gpio;
    GPIO_Module* port;
} bsp_i2c_hw_cfg_t;

static const bsp_i2c_hw_cfg_t g_bsp_i2c_cfgs[BSP_I2C_BUS_NUM] =
{
    {
        .base = BSP_I2C1,
        .irq = BSP_I2C1_EV_IRQ,
        .clk = BSP_I2C1_CLK,
        .gpiox = I2C1_GPIO_PORT_CLK,
        {
            .scl = I2C1_SCL_PIN,
            .sda = I2C1_SDA_PIN,
            .speed = GPIO_Speed_50MHz,
            .mode = GPIO_Mode_AF_OD,
        },
        .port = I2C1_GPIO_PORT,
    },
#ifdef BSP_I2C_MULTI
    {
        .base = BSP_I2C2,
        .irq = BSP_I2C2_EV_IRQ,
        .clk = BSP_I2C2_CLK,
        .gpiox = I2C2_GPIO_PORT_CLK,
        {
            .scl = I2C2_SCL_PIN,
            .sda = I2C2_SDA_PIN,
            .speed = GPIO_Speed_50MHz,
            .mode = GPIO_Mode_AF_OD,
        },
        .port = I2C2_GPIO_PORT,
    },
    {
        .base = BSP_I2C3,
        .irq = BSP_I2C3_EV_IRQ,
        .clk = BSP_I2C3_CLK,
        .gpiox = I2C3_GPIO_PORT_CLK,
        {
            .scl = I2C3_SCL_PIN,
            .sda = I2C3_SDA_PIN,
            .speed = GPIO_Speed_50MHz,
            .mode = GPIO_Mode_AF_OD,
        },
        .port = I2C3_GPIO_PORT,
    },
#endif
};

/**
 * @brief enable i2c scl clk
 * @param bus i2c bus
 * @retval NULL
 */
static void bsp_i2c_scl_clk_en(bsp_i2c_bus_t bus)
{
    RCC_EnableAPB2PeriphClk(g_bsp_i2c_cfgs[bus].gpiox, ENABLE);
}

/**
 * @brief enable i2c sda clk
 * @param bus i2c bus
 * @retval NULL
 */
static void bsp_i2c_sda_clk_en(bsp_i2c_bus_t bus)
{
    RCC_EnableAPB2PeriphClk(g_bsp_i2c_cfgs[bus].gpiox, ENABLE);
}


#ifdef I2C_MODE_SOFTWARE

#define I2C_DELAY_TIMES             2
#define GPIO_CFG_OD_OUTPUT          7
#define GPIO_CFG_PULL_UP_INPUT      8
#define GPIO_SDA_REG_POS            20
#define I2C_SET_HIGH                GPIO_SetBits
#define I2C_SET_LOW                 GPIO_ResetBits
#define I2C_GET_DATA                GPIO_ReadInputDataBit
#define I2C_READ_BIT                0X01
#define I2C_WRITE_BIT               0x00

typedef void (*bsp_soft_i2c_set_sda_output_t)(void);
typedef void (*bsp_soft_i2c_set_sda_input_t)(void);

static void bsp_soft_i2c_delay10us(volatile uint32_t us)
{
    volatile uint32_t i;

    for(i = 0; i < us; i++);
}

static void bsp_soft_i2c1_set_sda_output(void)
{
    I2C1_GPIO_PORT->PL_CFG &= 0XFF0FFFFF;
    I2C1_GPIO_PORT->PL_CFG |= GPIO_CFG_OD_OUTPUT << GPIO_SDA_REG_POS;
}

static void bsp_soft_i2c1_set_sda_input(void)
{
    I2C1_GPIO_PORT->PL_CFG &= 0XFF0FFFFF;
    I2C1_GPIO_PORT->PL_CFG |= GPIO_CFG_PULL_UP_INPUT << GPIO_SDA_REG_POS;
}

static void bsp_soft_i2c2_set_sda_output(void)
{
    I2C2_GPIO_PORT->PL_CFG &= 0XFF0FFFFF;
    I2C2_GPIO_PORT->PL_CFG |= GPIO_CFG_OD_OUTPUT << GPIO_SDA_REG_POS;
}

static void bsp_soft_i2c2_set_sda_input(void)
{
    I2C2_GPIO_PORT->PL_CFG &= 0XFF0FFFFF;
    I2C2_GPIO_PORT->PL_CFG |= GPIO_CFG_PULL_UP_INPUT << GPIO_SDA_REG_POS;
}

static void bsp_soft_i2c3_set_sda_output(void)
{
    I2C3_GPIO_PORT->PL_CFG &= 0XFF0FFFFF;
    I2C3_GPIO_PORT->PL_CFG |= GPIO_CFG_OD_OUTPUT << GPIO_SDA_REG_POS;
}

static void bsp_soft_i2c3_set_sda_input(void)
{
    I2C3_GPIO_PORT->PL_CFG &= 0XFF0FFFFF;
    I2C3_GPIO_PORT->PL_CFG |= GPIO_CFG_PULL_UP_INPUT << GPIO_SDA_REG_POS;
}

bsp_soft_i2c_set_sda_output_t bsp_soft_i2c_set_sda_output[BSP_I2C_BUS_NUM] = {bsp_soft_i2c1_set_sda_output, bsp_soft_i2c2_set_sda_output, bsp_soft_i2c3_set_sda_output};
bsp_soft_i2c_set_sda_input_t bsp_soft_i2c_set_sda_input[BSP_I2C_BUS_NUM] = {bsp_soft_i2c1_set_sda_input, bsp_soft_i2c2_set_sda_input, bsp_soft_i2c3_set_sda_input};

static void bsp_soft_i2c_start(bsp_i2c_bus_t bus)
{
    (*bsp_soft_i2c_set_sda_output[bus])();
    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
}

static void bsp_soft_i2c_stop(bsp_i2c_bus_t bus)
{
    (*bsp_soft_i2c_set_sda_output[bus])();
    I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
}

static uint8_t bsp_soft_i2c_rev_ack(bsp_i2c_bus_t bus)
{
    uint8_t ack = 0;

    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
     (*bsp_soft_i2c_set_sda_input[bus])();
    ack = I2C_GET_DATA(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);

    return ack;
}

static uint8_t bsp_soft_i2c_wait_ack(bsp_i2c_bus_t bus)
{
    uint8_t ack = 1;
    uint8_t i;  

    for(i = 0; i < 100; i++)
    {
        ack = bsp_soft_i2c_rev_ack(bus);
        if(!ack)
        {
            break;
        }
        if(i == 99)
        {
            bsp_soft_i2c_stop(bus);
            return 1;
        }
    }

    return RETVAL(E_OK);
}

static void bsp_soft_i2c_send_ack(bsp_i2c_bus_t bus)
{
    (*bsp_soft_i2c_set_sda_output[bus])();
    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
}

static void bsp_soft_i2c_send_no_ack(bsp_i2c_bus_t bus)
{
    (*bsp_soft_i2c_set_sda_output[bus])();
    I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
    I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
    bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
}

static void bsp_soft_i2c_write_byte(bsp_i2c_bus_t bus, uint8_t data)
{
    uint8_t i;

    (*bsp_soft_i2c_set_sda_output[bus])();
    for(i = 0; i < 8; i++)
    {
        if (data & 0x80)
        {
            I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
        }
        else
        {
            I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda);
        }
        bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
        I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
        bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
        I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
        bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
        data = data << 1;
    }
}

static uint8_t bsp_soft_i2c_read_byte(bsp_i2c_bus_t bus)
{
    uint8_t i;
    uint8_t data = 0;

    for(i = 0; i < 8; i++)
    {
        I2C_SET_HIGH(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
        bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
        data = data << 1;
        bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
        bsp_soft_i2c2_set_sda_input();
        if(I2C_GET_DATA(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.sda))
        {
            data = data | 0x1;
        }
        I2C_SET_LOW(g_bsp_i2c_cfgs[bus].port, g_bsp_i2c_cfgs[bus].gpio.scl);
        bsp_soft_i2c_delay10us(I2C_DELAY_TIMES);
    }

    return data;
}

int32_t bsp_i2c_read(i2c_transfer_t* i2c, uint8_t reg_addr, uint8_t* data, uint16_t len)
{
    uint16_t i;

    if (i2c->bus > BSP_I2C_BUS_NUM)
    {
        return RETVAL(E_FAIL);
    }
    bsp_soft_i2c_start(i2c->bus);
    bsp_soft_i2c_write_byte(i2c->bus, (i2c->slave_addr<<1)|I2C_WRITE_BIT);
    if(bsp_soft_i2c_wait_ack(i2c->bus))
    {
        return RETVAL(E_BUS);
    }
    bsp_soft_i2c_write_byte(i2c->bus, reg_addr);
    if(bsp_soft_i2c_wait_ack(i2c->bus))
    {
        return RETVAL(E_BUS);
    }
    bsp_soft_i2c_start(i2c->bus);
    bsp_soft_i2c_write_byte(i2c->bus, (i2c->slave_addr<<1)|I2C_READ_BIT);
    if(bsp_soft_i2c_wait_ack(i2c->bus))
    {
        return RETVAL(E_BUS);
    }
    for(i = 0; i < len; i++ )
    {
        *data = bsp_soft_i2c_read_byte(i2c->bus);
        if(i + 1 < len)
        {
            data++;
            bsp_soft_i2c_send_no_ack(i2c->bus);
        }
    }
    bsp_soft_i2c_send_ack(i2c->bus);
    bsp_soft_i2c_stop(i2c->bus);

    return RETVAL(E_OK);
}

int32_t bsp_i2c_write(i2c_transfer_t* i2c, uint8_t reg_addr, uint8_t* data, uint16_t len)
{
    uint16_t i;

    if (i2c->bus > BSP_I2C_BUS_NUM)
    {
        return RETVAL(E_FAIL);
    }
    bsp_soft_i2c_start(i2c->bus);
    bsp_soft_i2c_write_byte(i2c->bus, (i2c->slave_addr<<1)|I2C_WRITE_BIT);
    if(bsp_soft_i2c_wait_ack(i2c->bus))
    {
        return RETVAL(E_BUS);
    }
    bsp_soft_i2c_write_byte(i2c->bus, reg_addr);
    if(bsp_soft_i2c_wait_ack(i2c->bus))
    {
        return RETVAL(E_BUS);
    }
    for(i = 0; i < len; i++)
    {
        bsp_soft_i2c_write_byte(i2c->bus, *data);
        data ++;
        if(bsp_soft_i2c_wait_ack(i2c->bus))
        {
            return RETVAL(E_BUS);
        }
    }
    bsp_soft_i2c_stop(i2c->bus);

    return RETVAL(E_OK);
}

void bsp_i2c_init(bsp_i2c_bus_t bus)
{
    GPIO_InitType gpio;

    bsp_i2c_scl_clk_en(bus);
    bsp_i2c_sda_clk_en(bus);

    gpio.Pin = g_bsp_i2c_cfgs[bus].gpio.scl | g_bsp_i2c_cfgs[bus].gpio.sda;
    gpio.GPIO_Speed = g_bsp_i2c_cfgs[bus].gpio.speed;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitPeripheral(g_bsp_i2c_cfgs[bus].port, &gpio);
}

#else 

static void bsp_i2c_io_remap(bsp_i2c_bus_t bus)
{
    if (bus == BSP_I2C_BUS1)
    {
        ;
    }
    else if (bus == BSP_I2C_BUS2)
    {
        ;
    }
    else if (bus == BSP_I2C_BUS3)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
        GPIO_ConfigPinRemap(GPIO_RMP2_I2C3, ENABLE);
        GPIO_ConfigPinRemap(GPIO_RMP3_I2C3, ENABLE);
    }
}

/**
 * @brief enable i2c clk
 * @param bus i2c bus
 * @retval NULL
 */
static void bsp_i2c_peripheral_clk_en(bsp_i2c_bus_t bus)
{
    if ((bus == BSP_I2C_BUS1) || (bus == BSP_I2C_BUS2))
    {
        RCC_EnableAPB1PeriphClk(g_bsp_i2c_cfgs[bus].clk, ENABLE);
    }
    else if (bus == BSP_I2C_BUS3)
    {
        RCC_EnableAPB2PeriphClk(g_bsp_i2c_cfgs[bus].clk, ENABLE);
    }
}
/**
 * @brief config i2c err int clk
 * @param bus i2c bus
 * @retval NULL
 */
static void bsp_i2c_nvic_init(bsp_i2c_bus_t bus)
{
    NVIC_InitType nvic;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 2;
    if (bus == BSP_I2C_BUS1)
    {
        nvic.NVIC_IRQChannel = I2C1_ER_IRQn;
    }
    else if (bus == BSP_I2C_BUS2)
    {
        nvic.NVIC_IRQChannel = I2C2_ER_IRQn;
    }
    else if (bus == BSP_I2C_BUS3)
    {
        nvic.NVIC_IRQChannel = I2C3_ER_IRQn;
    }
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

/**
 * @brief i2c init
 * @param bus i2c channel
 */
void bsp_i2c_init(bsp_i2c_bus_t bus)
{
    I2C_InitType type;
    GPIO_InitType gpio;

    bsp_i2c_peripheral_clk_en(bus);
    I2C_DeInit(g_bsp_i2c_cfgs[bus].base);

    bsp_i2c_scl_clk_en(bus);
    bsp_i2c_sda_clk_en(bus);

    bsp_i2c_io_remap(bus);

    gpio.Pin = g_bsp_i2c_cfgs[bus].gpio.scl | g_bsp_i2c_cfgs[bus].gpio.sda;
    gpio.GPIO_Speed = g_bsp_i2c_cfgs[bus].gpio.speed;
    gpio.GPIO_Mode = g_bsp_i2c_cfgs[bus].gpio.mode;
    GPIO_InitPeripheral(g_bsp_i2c_cfgs[bus].port, &gpio);

    I2C_DeInit(g_bsp_i2c_cfgs[bus].base);

    type.ClkSpeed = I2C_SPEED;
    type.BusMode = I2C_BUSMODE_I2C;
    type.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    type.OwnAddr1 = 0x00;
    type.AckEnable = I2C_ACKEN;
    type.AddrMode = I2C_ADDR_MODE_7BIT;
    I2C_Init(g_bsp_i2c_cfgs[bus].base, &type);

    bsp_i2c_nvic_init(bus);
    I2C_ConfigInt(g_bsp_i2c_cfgs[bus].base, I2C_INT_ERR | I2C_INT_EVENT, ENABLE);

    I2C_Enable(g_bsp_i2c_cfgs[bus].base, ENABLE);
}

static void bsp_i2c_stop(I2C_Module* i2c, bsp_i2c_bus_t bus)
{
    I2C_ConfigAck(i2c, DISABLE);
    I2C_GenerateStop(i2c, ENABLE);
    bsp_i2c_init(bus);
}

static int32_t bsp_i2c_get_flag(I2C_Module* i2c, uint32_t flag)
{
    uint32_t time_out_period = I2C_LONG_TIMEOUT;
    while (I2C_GetFlag(i2c, flag))
    {
        if (time_out_period-- == 0)
        {
            return RETVAL(E_TIMEOUT);
        }
    }
    return RETVAL(E_OK);
}

static int32_t bsp_i2c_check_event(I2C_Module* i2c, uint32_t event)
{
    uint32_t time_out_period = I2C_LONG_TIMEOUT;
    while (!I2C_CheckEvent(i2c, event))
    {
        if (time_out_period-- == 0)
        {
            return RETVAL(E_TIMEOUT);
        }
    }
    return RETVAL(E_OK);
}

/**
 * @brief i2c send bytes
 * @param i2c transfer
 * @param reg:register address
 * @param data
 * @retval RETVAL
 */
static int32_t bsp_i2c_send_bytes(i2c_transfer_t* i2c, uint8_t reg, uint8_t* data, uint16_t len)
{
    int32_t ret;
    ret = bsp_i2c_get_flag(g_bsp_i2c_cfgs[i2c->bus].base, I2C_FLAG_BUSY);
    if (ret != E_OK)
    {
        LOG_E("[bsp_i2c][bsp_i2c_send_bytes]I2C BUSY!!!\r\n");
        return ret;
    }

    I2C_ConfigAck(g_bsp_i2c_cfgs[i2c->bus].base, ENABLE);
    I2C_GenerateStart(g_bsp_i2c_cfgs[i2c->bus].base, ENABLE);
    ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_MODE_FLAG);
    if (ret != E_OK) //EV5
    {
        LOG_E("[bsp_i2c][bsp_i2c_send_bytes]generate start failed!!!\r\n");
        return ret;
    }

    I2C_SendAddr7bit(g_bsp_i2c_cfgs[i2c->bus].base, i2c->slave_addr, I2C_DIRECTION_SEND);
    ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_TXMODE_FLAG);
    if (ret != E_OK) //EV6
    {
        LOG_E("[bsp_i2c][bsp_i2c_send_bytes]send slave address failed!!!\r\n");
        return ret;
    }

    I2C_SendData(g_bsp_i2c_cfgs[i2c->bus].base, reg);
    ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_DATA_SENDED);
    if (ret != E_OK) // EV8
    {
        LOG_E("[bsp_i2c][bsp_i2c_send_bytes]send write address failed!!!\r\n");
        return ret;
    }

    while (len > 0)
    {
        ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_DATA_SENDED);
        if (ret != E_OK) // EV8
        {
            LOG_E("[bsp_i2c][bsp_i2c_send_bytes]send data failed!!!\r\n");
            return ret;
        }
        I2C_SendData(g_bsp_i2c_cfgs[i2c->bus].base, *data);
        data++;
        len--;
    }

    I2C_GenerateStop(g_bsp_i2c_cfgs[i2c->bus].base, ENABLE);
    return RETVAL(E_OK);
}

/**
 * @brief i2c recv bytes
 * @param i2c transfer
 * @param reg:register address
 * @param data
 * @retval RETVAL
 */
static int32_t bsp_i2c_recv_bytes(i2c_transfer_t* i2c, uint8_t reg, uint8_t* data, uint16_t len)
{
    int32_t ret;
    ret = bsp_i2c_get_flag(g_bsp_i2c_cfgs[i2c->bus].base, I2C_FLAG_BUSY);
    if (ret != E_OK)
    {
        LOG_E("[bsp_i2c][bsp_i2c_recv_bytes]I2C BUSY!!!\r\n");
        return ret;
    }
    I2C_ConfigAck(g_bsp_i2c_cfgs[i2c->bus].base, ENABLE);
    I2C_GenerateStart(g_bsp_i2c_cfgs[i2c->bus].base, ENABLE);
    ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_MODE_FLAG);
    if (ret != E_OK) //EV5
    {
        LOG_E("[bsp_i2c][bsp_i2c_recv_bytes]generate start failed!!!\r\n");
        return ret;
    }
    I2C_SendAddr7bit(g_bsp_i2c_cfgs[i2c->bus].base, i2c->slave_addr, I2C_DIRECTION_SEND);LOG_D("in22\r\n");
    ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_TXMODE_FLAG);LOG_D("in23\r\n");
    if (ret != E_OK) //EV6
    {
        LOG_E("[bsp_i2c][bsp_i2c_recv_bytes]send slave address failed!!!\r\n");
        return ret;
    }
    I2C_SendData(g_bsp_i2c_cfgs[i2c->bus].base, reg);
    ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_DATA_SENDED);
    if (ret != E_OK) // EV8
    {
        LOG_E("[bsp_i2c][bsp_i2c_recv_bytes]send write address failed!!!\r\n");
        return ret;
    }
    I2C_GenerateStart(g_bsp_i2c_cfgs[i2c->bus].base, ENABLE);
    ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_MODE_FLAG);
    if (ret != E_OK) //EV5
    {
        LOG_E("[bsp_i2c][bsp_i2c_recv_bytes]generate start failed!!!\r\n");
        return ret;
    }
    I2C_SendAddr7bit(g_bsp_i2c_cfgs[i2c->bus].base, i2c->slave_addr, I2C_DIRECTION_RECV);
    ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_RXMODE_FLAG);
    if (ret != E_OK) //EV6
    {
        LOG_E("[bsp_i2c][bsp_i2c_recv_bytes]send slave address failed!!!\r\n");
        return ret;
    }
    while (len > 0)
    {
        if (len == 1) {
            I2C_ConfigAck(g_bsp_i2c_cfgs[i2c->bus].base, DISABLE);
            I2C_GenerateStop(g_bsp_i2c_cfgs[i2c->bus].base, ENABLE);
        }
        ret = bsp_i2c_check_event(g_bsp_i2c_cfgs[i2c->bus].base, I2C_EVT_MASTER_DATA_RECVD_FLAG);
        if (ret != E_OK)
        {
            LOG_E("[bsp_i2c][bsp_i2c_recv_bytes]i2c read data failed!!!\r\n");
            return ret;
        }
        *data = I2C_RecvData(g_bsp_i2c_cfgs[i2c->bus].base);
        data++;
        len--;
    }

    return RETVAL(E_OK);
}
/**
 * @brief i2c read
 * @param reg:register address
 * @param data
 * @param len
 * @retval RETVAL
 */
int32_t bsp_i2c_read(i2c_transfer_t* i2c, uint8_t reg, uint8_t* data, uint16_t len)
{
    int32_t ret = RETVAL(E_FAIL);
    if (len > 0)
    {
        ret = bsp_i2c_recv_bytes(i2c, reg, data, len);
    }
    return ret;
}
/**
 * @brief i2c write
 * @param reg:register address
 * @param data
 * @param len
 * @retval RETVAL
 */
int32_t bsp_i2c_write(i2c_transfer_t* i2c, uint8_t reg, uint8_t* data, uint16_t len)
{
    int32_t ret = RETVAL(E_FAIL);
    if (len > 0)
    {
        ret = bsp_i2c_send_bytes(i2c, reg, data, len);
    }

    return ret;
}

void I2C2_ER_IRQHandler(void)
{
    bsp_i2c_stop(I2C2, BSP_I2C_BUS2);
}

void I2C1_ER_IRQHandler(void)
{
    bsp_i2c_stop(I2C1, BSP_I2C_BUS1);
}

#endif
