#ifndef  __BSP_SPI_H__
#define  __BSP_SPI_H__

typedef enum
{
    BSP_SPI_BUS1,
    BSP_SPI_BUS_NUM,
} bsp_spi_bus_t;

void bsp_spi_init(bsp_spi_bus_t bus);
int32_t bsp_spi_transfer(bsp_spi_bus_t spi_bus, uint8_t *tx_buf, int16_t len, uint8_t *rx_buf);
int32_t bsp_spi_transmit_byte(bsp_spi_bus_t spi_bus, uint8_t data);
int32_t bsp_spi_transmit_bytes(bsp_spi_bus_t spi_bus, uint8_t *tx_buf, int16_t len);

#endif
