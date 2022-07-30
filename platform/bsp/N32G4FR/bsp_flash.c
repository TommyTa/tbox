/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bsp_flash.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/
#include "n32g4fr.h"
#include "errorno.h"
#include "bsp_flash.h"

#define FLASH_PAGE_SIZE         2048
#define FLASH_OB_DATA_ADDR      0x1FFFF804
#define FLASH_OB_DATA1_ADDR     0x1FFFF806

static inline int32_t bsp_flash_sts_to_errno(FLASH_STS sts)
{
    int32_t ret = RETVAL(E_OK);

    if (sts == FLASH_TIMEOUT)
    {
        ret = RETVAL(E_TIMEOUT);
    }
    else if (sts == FLASH_BUSY)
    {
        ret = RETVAL(E_BUSY);
    }
    else if(sts == FLASH_ERR_ADD)
    {
        ret = RETVAL(E_INVAL_ADDR);
    }
    else if (sts != FLASH_COMPL)
    {
        ret = RETVAL(E_FAIL);
    }

    return ret;
}

int32_t bsp_flash_erase_page(uint32_t addr)
{
    int32_t ret = RETVAL(E_OK);
    FLASH_STS sts;

    FLASH_Unlock();
    sts = FLASH_EraseOnePage(addr);
    ret = bsp_flash_sts_to_errno(sts);
    FLASH_Lock();

    return ret;
}

int32_t bsp_flash_write_words(uint32_t addr, uint32_t *data, uint32_t len)
{
    FLASH_STS err;
    int32_t ret = RETVAL(E_OK);
    uint32_t write_len;
    uint32_t i;

    if (len % sizeof(uint32_t) != 0)
    {
        return RETVAL(E_INVAL_LEN);
    }
    write_len = len / sizeof(uint32_t);

    FLASH_Unlock();
    for (i = 0; i < write_len; i++)
    {
        err = FLASH_ProgramWord(addr + i * sizeof(uint32_t), data[i]);
        if (err != FLASH_COMPL)
        {
            ret = RETVAL(E_FAIL);
            break;
        }
    }
    FLASH_Lock();

    return ret;
}

uint8_t bsp_flash_read_byte(uint32_t addr)
{
    return *(__IO uint8_t *)addr;
}

void bsp_flash_read_bytes(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++)
    {
        *(data + i) = bsp_flash_read_byte(addr + i * sizeof(uint8_t));
    }
}

uint32_t bsp_flash_read_word(uint32_t addr)
{
    return *(__IO uint32_t *)addr;
}

void bsp_flash_read_words(uint32_t addr, uint32_t *data, uint32_t word_len)
{
    uint32_t i;

    for (i = 0; i < word_len; i++)
    {
        *(data + i) = bsp_flash_read_word(addr + i * sizeof(uint32_t));
    }
}

int32_t bsp_flash_carry(uint32_t t_addr, uint32_t s_addr, uint32_t size)
{
    FLASH_STS sts;
    int32_t ret = RETVAL(E_OK);
    uint32_t word_len;
    uint32_t page_len;
    uint32_t i;

    word_len = (size / sizeof(uint32_t));
    page_len = (size % FLASH_PAGE_SIZE) == 0 ?
                (size / FLASH_PAGE_SIZE) : (size / FLASH_PAGE_SIZE) + 1;
    
    for (i = 0; i < page_len; i++)
    {
        bsp_flash_erase_page(t_addr + i * FLASH_PAGE_SIZE);
    }

    FLASH_Unlock();
    
    for (i = 0; i < word_len; i++)
    {
        sts = FLASH_ProgramWord(t_addr + i * sizeof(uint32_t),
                                bsp_flash_read_word(s_addr + i * sizeof(uint32_t)));
        if (sts != FLASH_COMPL)
        {
            break;
        }
    }

    FLASH_Lock();

    ret = bsp_flash_sts_to_errno(sts);
    if (ret == RETVAL(E_OK))
    {
        for (i = 0; i < page_len; i++)
        {
            bsp_flash_erase_page(s_addr + i * FLASH_PAGE_SIZE);
        }
    }
    
    return ret;
}

void bsp_flash_erase_nv(void)
{
    FLASH_Unlock();
    FLASH_EraseOB();
    FLASH_Lock();
}

/**
 * @brief OB Data bit is Half word
 * @param data (Data1 << 8)| Data0
 */
int32_t bsp_flash_write_nv(uint16_t data)
{
    FLASH_STS sts;
    int32_t ret = RETVAL(E_OK);

    bsp_flash_erase_nv();

    FLASH_Unlock();
    //OB Data only Low eight bit effective.  high 8bit  [low 8bit]
    sts = FLASH_ProgramOBData(FLASH_OB_DATA_ADDR, 
                            (((data >> 8) & 0x00FF) << 16) | (data & 0x00FF));

    FLASH_Lock();

    ret = bsp_flash_sts_to_errno(sts);
    return ret;
}

uint16_t bsp_flash_read_nv(void)
{
    return ((bsp_flash_read_byte(FLASH_OB_DATA1_ADDR) << 8) |
            bsp_flash_read_byte(FLASH_OB_DATA_ADDR));
}
