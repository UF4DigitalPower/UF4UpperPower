# UF4UpperPower STM32H743 固件

这是基于 STM32H743ZITX 的数字电源上位控制/显示固件。工程使用 STM32 HAL/CubeMX 外设初始化，CMake + `arm-none-eabi-gcc` 构建，目标产物为 `UF4UpperPower.elf/.hex/.bin`。

## 当前硬件与显示方向

- MCU: STM32H743ZITX, Cortex-M7, I-Cache/D-Cache 均开启。
- LCD 控制链路: LTDC + DMA2D + ST7701 面板初始化。
- LTDC 物理 framebuffer: `480 x 640`, RGB565。
- GUI 逻辑坐标: 横屏 `640 x 480`。
- 外部 SDRAM:
  - `0xC0000000` 起始保留 4 MB 给双 framebuffer。
  - `.sdram` 大缓存放在 `SDRAM_SCRATCH`, 从 `0xC0400000` 开始。

## 目录结构

```text
Core/
  Inc, Src             CubeMX/HAL 生成的外设初始化、main、时钟、MPU、中断
Drivers/              STM32 HAL/CMSIS 驱动
UF4BSP/
  bsp_lcd.*           LCD 抽象、LTDC framebuffer、DMA2D blit、显示方向映射
  bsp_font.*          Teko 字体渲染、字体缓存、MDMA 预旋转缓存
  bsp_st7701.*        ST7701 面板初始化
UF4APP/
  GUI/                数字电源界面、脏矩形刷新、性能显示
  TELECOM/            UF4COM 通信客户端、USART1 收发和寄存器缓存
  USER/               编码器/按键设定值输入
UF4COM/               通信协议子模块
script/, storage/     工程辅助文件
```

## 构建

```powershell
cmake --build cmake-build-debug
```

构建完成后会在 `cmake-build-debug` 里生成 `.elf`, `.hex`, `.bin`。

## 启动流程

`Core/Src/main.c` 中的主要初始化顺序：

1. 配置 MPU，开启 I-Cache/D-Cache。
2. 初始化 GPIO、DMA、MDMA、DMA2D、FMC、LTDC、QSPI、SPI、UART、TIM。
3. `LCD_Init()` 配置 LCD 抽象层和 framebuffer。
4. `ST7701Init()` 初始化面板。
5. `SetpointInput_Init()` 启动两个旋转编码器。
6. `HAL_TIM_Base_Start_IT(&htim6)` 用 TIM6 周期更新编码器/按键。
7. `GUI_Init()` 绘制静态界面并初始化双缓冲内容。

主循环按 `APP_GUI_UPDATE_PERIOD_MS = 20 ms` 尝试刷新 GUI；当前模拟遥测更新周期为 `APP_TELEMETRY_UPDATE_PERIOD_MS = 50 ms`，对应 20 Hz。数据流 20 Hz 是系统约束，不通过修改数据频率来换帧率。

## GUI 架构

GUI 位于 `UF4APP/GUI/gui.c`，当前布局是横屏 `640 x 480`：

- 顶部: VIN/IIN/PIN/EFF/FAN。
- 左侧大块: VO/IO/PO。
- 右侧: VSET/ISET、状态块、温度块、FPS/CPU 性能块。
- 使用 dirty mask 判断哪些区域需要重画。
- 重画前通过 `LCD_CopyRectFromFrontToDraw()` 把未变化区域从 front buffer 同步到 draw buffer。
- 完成后 `LCD_Present()` 切换显示 buffer。

这种方式避免每帧整屏重画，主要 CPU 压力来自数字字体绘制和缓存/总线同步。

## LCD 与 DMA2D 热路径

`UF4BSP/bsp_lcd.c` 负责把 GUI 逻辑横屏坐标映射到 LTDC 物理竖屏 framebuffer。

关键路径：

- `LCD_BlitRectRGB565()`：直接 RGB565 矩形拷贝。
- `LCD_BlitRotatedRectRGB565()`：源数据需要先做 DCache clean，再由 DMA2D 搬到 framebuffer。
- `LCD_BlitRotatedRectRGB565Clean()`：源数据已经是干净缓存，跳过源 DCache clean，运行时开销更低。
- `LCD_CopyRectFromFrontToDraw()`：DMA2D 从 front buffer 拷贝脏矩形背景到 draw buffer。

曾测试过运行时 MDMA linked-list 旋转 blit，但实际 CPU 占用升高且帧率下降。因此运行时不启用 MDMA 旋转，`LCD_MDMA_ROTATE_ENABLE` 保持关闭。

## 字体缓存策略

`UF4BSP/bsp_font.c` 使用 Teko 字体，并针对高频数字绘制做预旋转缓存：

- 大数字缓存: size 144, cell `60 x 104`, 用于 VO/IO/PO。
- 设定值缓存: size 34, cell `16 x 34`, 用于 VSET/ISET。
- 缓存字符: `0-9`, `.`, `-`, 空格。
- 字体先渲染到逻辑横屏临时 buffer，再预旋转到物理方向缓存。
- 预旋转阶段使用 MDMA linked-list，仅在缓存构建时运行。
- 运行时直接 `LCD_BlitRotatedRectRGB565Clean()` + DMA2D 搬缓存，不再逐像素绘制或每帧 MDMA 旋转。

这个设计的目标是把旋转成本从每帧热路径挪到初始化/首次使用阶段。

## MDMA 使用边界

当前 MDMA 的使用原则：

- 可以用于字体缓存预旋转。
- 不用于每帧 LCD blit 热路径。
- 不改变遥测数据流频率。
- 若 MDMA 预旋转失败，字体缓存会回退到 CPU 旋转，保证界面仍可显示。

注意：`Core/Src/mdma.c` 中仍有整屏旋转实验代码和测试 buffer，当前 GUI 热路径不依赖它。后续如果清理 SDRAM 占用，可以评估是否移除这些实验 buffer。

## 编码器输入

`UF4APP/USER/setpoint_input.c` 管理 VSET/ISET 两路编码器和按键：

- TIM4: VSET 编码器。
- TIM2: ISET 编码器。
- 按键切换当前编辑位。
- 当前按一格机械 detent 只变化 1 个单位步进，通过 `SETPOINT_ENCODER_COUNTS_PER_STEP = 2` 对硬件计数做累积折算。

## 通信

`UF4APP/TELECOM/uf4_power_client.c` 使用 USART1 和 UF4COM 协议：

- 支持读/写 U16 寄存器。
- 支持 stream start/stop。
- UART RX 使用中断逐字节喂入 parser。
- 接收到 stream/read/write 响应后更新本地寄存器缓存和数据变化标志。

## 性能优化记录

当前显示性能优化的结论：

- 不改 20 Hz 数据流。
- 不在运行时使用 MDMA 做整屏/矩形旋转。
- 大字体和设定值小字体预旋转缓存收益明显。
- 缓存拆成不同用途的区域，便于查找和避免混用。
- DMA2D 是运行时搬运主力；MDMA 只承担缓存生成时的旋转工作。
- DCache 一致性非常关键：CPU 写给 DMA 读之前要 clean，DMA 写回后 CPU 可能读取的区域要 invalidate。

## 后续可做

- 清理 `Core/Src/mdma.c` 中未参与正式路径的整屏旋转测试 buffer，释放 SDRAM scratch。
- 继续扩大预旋转缓存覆盖范围，但应优先缓存高频、尺寸大、背景固定的字体。
- 将真实遥测接入 GUI 数据源后，保持 20 Hz 数据节奏，只优化绘制路径。
