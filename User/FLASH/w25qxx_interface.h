#ifndef W25QXX_INTERFACE_H
#define W25QXX_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t w25qxx_interface_spi_qspi_init(void);
uint8_t w25qxx_interface_spi_qspi_deinit(void);
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
                                             uint8_t data_line);
void w25qxx_interface_delay_ms(uint32_t ms);
void w25qxx_interface_delay_us(uint32_t us);
void w25qxx_interface_debug_print(const char *const fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
