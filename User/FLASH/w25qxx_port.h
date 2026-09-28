#ifndef W25QXX_PORT_H
#define W25QXX_PORT_H

#include <stdint.h>

#include "w25q64.h"

#ifdef __cplusplus
extern "C" {
#endif

extern w25qxx_handle_t g_w25qxx;
extern uint8_t g_w25qxx_init_status;

uint8_t W25Qxx_Init(void);
/* 非阻塞 4K 擦除：Start 只发命令，后续用 IsBusy 轮询 WIP。 */
uint8_t W25Qxx_SectorErase4KStart(uint32_t address);
uint8_t W25Qxx_IsBusy(uint8_t *busy);

#ifdef __cplusplus
}
#endif

#endif
