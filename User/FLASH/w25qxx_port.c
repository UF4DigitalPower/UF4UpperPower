#include "w25qxx_port.h"

#include "w25qxx_interface.h"

#define W25QXX_PORT_CMD_WRITE_ENABLE       0x06U
#define W25QXX_PORT_CMD_READ_STATUS_REG1   0x05U
#define W25QXX_PORT_CMD_SECTOR_ERASE_4K    0x20U

w25qxx_handle_t g_w25qxx;
uint8_t g_w25qxx_init_status = 0xFFU;

uint8_t W25Qxx_Init(void)
{
    DRIVER_W25QXX_LINK_INIT(&g_w25qxx, w25qxx_handle_t);
    DRIVER_W25QXX_LINK_SPI_QSPI_INIT(&g_w25qxx, w25qxx_interface_spi_qspi_init);
    DRIVER_W25QXX_LINK_SPI_QSPI_DEINIT(&g_w25qxx, w25qxx_interface_spi_qspi_deinit);
    DRIVER_W25QXX_LINK_SPI_QSPI_WRITE_READ(&g_w25qxx, w25qxx_interface_spi_qspi_write_read);
    DRIVER_W25QXX_LINK_DELAY_MS(&g_w25qxx, w25qxx_interface_delay_ms);
    DRIVER_W25QXX_LINK_DELAY_US(&g_w25qxx, w25qxx_interface_delay_us);
    DRIVER_W25QXX_LINK_DEBUG_PRINT(&g_w25qxx, w25qxx_interface_debug_print);

    (void)w25qxx_set_type(&g_w25qxx, W25Q256);
    (void)w25qxx_set_interface(&g_w25qxx, W25QXX_INTERFACE_SPI);
    (void)w25qxx_set_dual_quad_spi(&g_w25qxx, W25QXX_BOOL_FALSE);

    g_w25qxx_init_status = w25qxx_init(&g_w25qxx);
    return g_w25qxx_init_status;
}

uint8_t W25Qxx_SectorErase4KStart(uint32_t address)
{
    if(g_w25qxx_init_status != 0U)
    {
        return 1U;
    }

    if(w25qxx_interface_spi_qspi_write_read(
           W25QXX_PORT_CMD_WRITE_ENABLE, 1U,
           0U, 0U, 0U, 0U, 0U, 0U, 0U,
           NULL, 0U, NULL, 0U, 0U) != 0U)
    {
        return 1U;
    }

    return w25qxx_interface_spi_qspi_write_read(
        W25QXX_PORT_CMD_SECTOR_ERASE_4K, 1U,
        address, 1U, 3U, 0U, 0U, 0U, 0U,
        NULL, 0U, NULL, 0U, 0U);
}

uint8_t W25Qxx_IsBusy(uint8_t *busy)
{
    uint8_t status = 0U;

    if(busy == NULL || g_w25qxx_init_status != 0U)
    {
        return 1U;
    }

    if(w25qxx_interface_spi_qspi_write_read(
           W25QXX_PORT_CMD_READ_STATUS_REG1, 1U,
           0U, 0U, 0U, 0U, 0U, 0U, 0U,
           NULL, 0U, &status, 1U, 1U) != 0U)
    {
        return 1U;
    }

    *busy = (status & 0x01U) != 0U ? 1U : 0U;
    return 0U;
}
