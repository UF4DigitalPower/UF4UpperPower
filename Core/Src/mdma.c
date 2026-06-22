/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : mdma.c
  * Description        : This file provides code for the configuration
  *                      of all the requested global MDMA transfers.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "mdma.h"

/* USER CODE BEGIN 0 */
#define WW  480   // 源图像宽度（像素）
#define HH  640   // 源图像高度（像素）
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure MDMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

// 32 字节对齐！必须放在 AXI SRAM（0x24000000 区域）或 DTCM
// 如果在 main 外定义，加 __attribute__((section(".ARM.__at_0x24000000"))) 也行
__attribute__((section(".sdram"), aligned(32))) uint16_t src_buf[WW * HH];  // 源图像：横屏
__attribute__((section(".sdram"), aligned(32))) uint16_t dst_buf[HH * WW];  // 目标图像：竖屏（旋转后）
__attribute__((section(".sdram"), aligned(32))) MDMA_LinkNodeTypeDef node_array[HH];
/* USER CODE END 1 */
MDMA_HandleTypeDef hmdma_mdma_channel0_sw_0;

/**
  * Enable MDMA controller clock
  * Configure MDMA for global transfers
  *   hmdma_mdma_channel0_sw_0
  */
void MX_MDMA_Init(void)
{

  /* MDMA controller clock enable */
  __HAL_RCC_MDMA_CLK_ENABLE();
  /* Local variables */

  /* Configure MDMA channel MDMA_Channel0 */
  /* Configure MDMA request hmdma_mdma_channel0_sw_0 on MDMA_Channel0 */
  hmdma_mdma_channel0_sw_0.Instance = MDMA_Channel0;
  hmdma_mdma_channel0_sw_0.Init.Request = MDMA_REQUEST_SW;
  hmdma_mdma_channel0_sw_0.Init.TransferTriggerMode = MDMA_FULL_TRANSFER;
  hmdma_mdma_channel0_sw_0.Init.Priority = MDMA_PRIORITY_HIGH;
  hmdma_mdma_channel0_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdma_mdma_channel0_sw_0.Init.SourceInc = MDMA_SRC_DEC_HALFWORD;
  hmdma_mdma_channel0_sw_0.Init.DestinationInc = MDMA_DEST_INC_HALFWORD;
  hmdma_mdma_channel0_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  hmdma_mdma_channel0_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  hmdma_mdma_channel0_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdma_mdma_channel0_sw_0.Init.BufferTransferLength = 2;
  hmdma_mdma_channel0_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  hmdma_mdma_channel0_sw_0.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  hmdma_mdma_channel0_sw_0.Init.SourceBlockAddressOffset = 0;
  hmdma_mdma_channel0_sw_0.Init.DestBlockAddressOffset = HH * 2;
  if (HAL_MDMA_Init(&hmdma_mdma_channel0_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* MDMA interrupt initialization */
  /* MDMA_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MDMA_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(MDMA_IRQn);

}
/* USER CODE BEGIN 2 */
void MDMA_Rotation90_Init(void)
{
    MDMA_LinkNodeConfTypeDef cfg;
    uint32_t i;

    // 先跑 CubeMX 生成的基础初始化（配好通道、时钟、NVIC 等）
    MX_MDMA_Init();

    // 逐个节点配置：每个节点 = 源图像的一行 → 目标图像的一列
    for (i = 0; i < HH; i++)
    {
        /* ========== 基础传输参数（和 CubeMX 里配的一致）========== */
        cfg.Init.Request              = MDMA_REQUEST_SW;
        cfg.Init.TransferTriggerMode  = MDMA_FULL_TRANSFER;
        cfg.Init.Priority             = MDMA_PRIORITY_HIGH;
        cfg.Init.Endianness           = MDMA_LITTLE_ENDIANNESS_PRESERVE;

        // 源：从每行末尾开始，向左递减读取
        cfg.Init.SourceInc            = MDMA_SRC_DEC_HALFWORD;
        // 目标：块内地址固定，靠 BlockOffset 跳到下一列
        cfg.Init.DestinationInc       = MDMA_DEST_INC_DISABLE;

        cfg.Init.SourceDataSize       = MDMA_SRC_DATASIZE_HALFWORD;
        cfg.Init.DestDataSize         = MDMA_DEST_DATASIZE_HALFWORD;
        cfg.Init.DataAlignment        = MDMA_DATAALIGN_PACKENABLE;
        cfg.Init.SourceBurst          = MDMA_SOURCE_BURST_SINGLE;
        cfg.Init.DestBurst            = MDMA_DEST_BURST_SINGLE;
        cfg.Init.BufferTransferLength = 2;          // 1 个 RGB565 像素

        // 源块间不偏移（行内连续读完即可）
        cfg.Init.SourceBlockAddressOffset = 0;
        // 目标块间偏移：跳到下一行同列位置 = 高度 × 2 字节
        cfg.Init.DestBlockAddressOffset   = HH * 2;

        /* ========== 每个节点独有的地址 ========== */
        // 源：第 i 行最后一个像素地址（从右往左读）
        // src_buf 是 uint16_t 数组，指针运算就是半字单位
        cfg.SrcAddress = (uint32_t)(&src_buf[(WW - 1) + i * WW]);

        // 目标：第 i 列第一个像素地址（从上往下写）
        cfg.DstAddress = (uint32_t)(&dst_buf[i]);

        // 块数量 = 图像宽度（每行有多少像素就传多少个块）
        cfg.BlockDataLength = 2;
        cfg.BlockCount = WW;

        /* ========== 创建节点并加入链表 ========== */
        HAL_MDMA_LinkedList_CreateNode(&node_array[i], &cfg);

        if (i == 0)
        {
            // 第一个节点：挂到句柄上，prev = NULL
            HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel0_sw_0, &node_array[i], 0);
        }
        else
        {
            // 后续节点：挂到前一个节点后面
            HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel0_sw_0, &node_array[i], &node_array[i-1]);
        }
    }
}

void MDMA_Rotation90_Start(void)
{
    // H7 的 D-Cache 必须手动维护！
    // 1. Clean：把 src_buf 的脏数据写回 SRAM，确保 DMA 读到最新
    SCB_CleanDCache_by_Addr((uint32_t *)src_buf, WW * HH * sizeof(uint16_t));

    // 2. Invalidate：让 dst_buf 的 Cache 行失效，避免 CPU 读到旧数据
    SCB_InvalidateDCache_by_Addr((uint32_t *)dst_buf, HH * WW * sizeof(uint16_t));

    // 启动链表传输（软件触发）
    // 参数：句柄、源起始地址、目标起始地址、单块字节数、块数量
    HAL_MDMA_Start(&hmdma_mdma_channel0_sw_0,
                   (uint32_t)src_buf,
                   (uint32_t)dst_buf,
                   2U,
                   WW);
}


/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

