#include "w25qxx_interface.h"

#include "main.h"
#include "spi.h"

#define W25QXX_SPI_TIMEOUT_MS  20U

static void w25qxx_cs_write(GPIO_PinState state)
{
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, state);
}

uint8_t w25qxx_interface_spi_qspi_init(void)
{
    w25qxx_cs_write(GPIO_PIN_SET);
    return (hspi1.State == HAL_SPI_STATE_READY) ? 0U : 1U;
}

uint8_t w25qxx_interface_spi_qspi_deinit(void)
{
    w25qxx_cs_write(GPIO_PIN_SET);
    return 0U;
}

uint8_t w25qxx_interface_spi_qspi_write_read(uint8_t instruction,
                                             uint8_t instruction_line,
                                             uint32_t address,
                                             uint8_t address_line,
                                             uint8_t address_len,
                                             uint32_t alternate,
                                             uint8_t alternate_line,
                                             uint8_t alternate_len,
                                             uint8_t dummy,
                                             uint8_t *in_buf,
                                             uint32_t in_len,
                                             uint8_t *out_buf,
                                             uint32_t out_len,
                                             uint8_t data_line)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t header[5];
    uint32_t header_len = 0U;
    uint32_t i;

    /* This adapter intentionally supports the driver's normal 1-1-1 SPI path. */
    if ((instruction_line > 1U) || (address_line > 1U) ||
        (alternate_line > 1U) || (data_line > 1U))
    {
        return 1U;
    }

    if (instruction_line != 0U)
    {
        header[header_len++] = instruction;
    }
    for (i = 0U; i < address_len; i++)
    {
        header[header_len++] = (uint8_t)(address >> (8U * (address_len - 1U - i)));
    }
    for (i = 0U; i < alternate_len; i++)
    {
        header[header_len++] = (uint8_t)(alternate >> (8U * (alternate_len - 1U - i)));
    }

    w25qxx_cs_write(GPIO_PIN_RESET);

    if (header_len != 0U)
    {
        status = HAL_SPI_Transmit(&hspi1, header, header_len, W25QXX_SPI_TIMEOUT_MS);
    }
    for (i = 0U; (status == HAL_OK) && (i < dummy); i++)
    {
        uint8_t discard;
        uint8_t fill = 0xFFU;
        status = HAL_SPI_TransmitReceive(&hspi1, &fill, &discard, 1U, W25QXX_SPI_TIMEOUT_MS);
    }
    if ((status == HAL_OK) && (in_len != 0U) && (in_buf != NULL))
    {
        status = HAL_SPI_Transmit(&hspi1, in_buf, in_len, W25QXX_SPI_TIMEOUT_MS);
    }
    if ((status == HAL_OK) && (out_len != 0U) && (out_buf != NULL))
    {
        status = HAL_SPI_Receive(&hspi1, out_buf, out_len, W25QXX_SPI_TIMEOUT_MS);
    }

    w25qxx_cs_write(GPIO_PIN_SET);
    return (status == HAL_OK) ? 0U : 1U;
}

void w25qxx_interface_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void w25qxx_interface_delay_us(uint32_t us)
{
    static uint8_t dwt_ready = 0U;
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000U);

    if (dwt_ready == 0U)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0U;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        dwt_ready = 1U;
        start = DWT->CYCCNT;
    }
    while ((DWT->CYCCNT - start) < ticks) {}
}

void w25qxx_interface_debug_print(const char *const fmt, ...)
{
    (void)fmt;
}
