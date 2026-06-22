# UF4UpperPower STM32H743 固件

这是基于 STM32H743ZITX 的数字电源上位控制/显示固件。工程使用 STM32 HAL/CubeMX 外设初始化，CMake + `arm-none-eabi-gcc` 构建，目标产物为 `UF4UpperPower.elf/.hex/.bin`。

## 当前硬件与显示方向

- MCU: STM32H743ZITX, Cortex-M7, I-Cache/D-Cache 均开启。
- LCD 控制链路: LTDC + DMA2D + ST7701 面板初始化。
- LTDC 物理 framebuffer: `480 x 640`, RGB565。
- GUI 逻辑坐标: 横屏 `640 x 480`。
- 外部 SDRAM:
  - `0xC0000000` 起始保留 4 MB 给双 framebuffer。
  - framebuffer MPU 配置为 non-cacheable，减少 LTDC/DMA2D 目标区 cache 维护。
  - `.sdram` 大缓存放在 `SDRAM_SCRATCH`, 从 `0xC0400000` 开始，MPU 配置为 cacheable。

## 目录结构

```text
Core/
  Inc, Src             CubeMX/HAL 生成的外设初始化、main、时钟、MPU、中断
Drivers/              STM32 HAL/CMSIS 驱动
UF4BSP/
  bsp_lcd.*           LCD 抽象、LTDC framebuffer、DMA2D blit、显示方向映射、性能计数
  bsp_font.*          Teko 字体渲染、字体缓存、tight blit、MDMA 预旋转缓存
  bsp_st7701.*        ST7701 面板初始化
UF4APP/
  GUI/                数字电源界面、脏矩形刷新、性能显示
  TELECOM/            UF4COM 通信客户端、USART1 收发和寄存器缓存
  USER/               VSET/ISET 编码器输入、5 键保护/输出面板输入
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

1. 配置 MPU，开启 I-Cache/D-Cache，并启用 DWT cycle counter。
2. 初始化 GPIO、DMA、MDMA、DMA2D、FMC、LTDC、QSPI、SPI、UART、TIM。
3. `LCD_Init()` 配置 LCD 抽象层和 framebuffer。
4. `ST7701Init()` 初始化面板。
5. `SetpointInput_Init()` 启动 VSET/ISET 两个旋转编码器。
6. `PanelKeys_Init()` 初始化 5 个面板按键。
7. `HAL_TIM_Base_Start_IT(&htim6)` 用 TIM6 周期更新编码器和面板按键。
8. `GUI_Init()` 绘制静态界面并初始化双缓冲内容。

主循环按 `APP_GUI_UPDATE_PERIOD_MS = 20 ms` 尝试刷新 GUI；当前模拟遥测更新周期为 `APP_TELEMETRY_UPDATE_PERIOD_MS = 50 ms`，对应 20 Hz。数据流 20 Hz 是系统约束，不通过修改数据频率来换帧率。

## GUI 架构

GUI 位于 `UF4APP/GUI/gui.c`，当前布局是横屏 `640 x 480`：

- 顶部: VIN/IIN/PIN/EFF/FAN。
- 左侧大块: VO/IO/PO。
- 右侧: VSET/ISET、OTP/OVP/OCP/OUT/BLE、状态块、温度块、FPS/CPU/GUI/DMA 性能块。
- 使用 dirty mask 判断哪些区域需要重画。
- `GUI_FRONT_COPY_ENABLE` 当前为 `0`，双缓冲 stale 区域通过重画同步，不走 front-to-draw DMA2D 补拷。
- 完成后 `LCD_Present()` 切换显示 buffer。

小字体显示做了两类优化：

- `LCD_DrawFontStringDMATight()` 根据 glyph 实际 `min_row/height` 裁掉上下空白，只 blit 实际字形区域。
- `GUI_DrawFixedSlotText()` 给小数字使用固定字符槽，避免 `1`、`.` 等窄字符导致数值左右跳动。

## LCD 与 DMA2D 热路径

`UF4BSP/bsp_lcd.c` 负责把 GUI 逻辑横屏坐标映射到 LTDC 物理竖屏 framebuffer。

关键路径：

- `LCD_BlitRectRGB565()`：直接 RGB565 矩形拷贝。
- `LCD_BlitRotatedRectRGB565()`：源数据需要先做 DCache clean，再由 DMA2D 搬到 framebuffer。
- `LCD_BlitRotatedRectRGB565Clean()`：源数据已经是干净缓存，跳过源 DCache clean，运行时开销更低。
- `LCD_CopyRectFromFrontToDraw()`：保留为可选 front-to-draw 补拷路径；当前 GUI 默认不使用。

framebuffer 所在 4 MB MPU 区域为 non-cacheable，因此对 framebuffer 目标地址的 DCache clean/invalidate 会被跳过。`.sdram` scratch/cache 区仍为 cacheable，字体缓存、临时 buffer 和 MDMA node 仍按 DMA 方向做 cache 维护。

`LCD_PerfGet()` / `LCD_PerfReset()` 提供 DMA2D busy-wait 和 DCache 维护周期统计。屏幕性能块当前显示：

```text
FPS
CPU  主循环墙钟 busy 百分比
GUI  GUI_Update() DWT 周期百分比
DMA  DMA2D 轮询等待 DWT 周期百分比
```

## 字体缓存策略

`UF4BSP/bsp_font.c` 使用 Teko 字体，并针对高频数字绘制做预旋转缓存：

- 大数字缓存: size 144, cell `60 x 104`, 用于 VO/IO/PO。
- 设定值缓存: size 34, cell `16 x 34`, 用于 VSET/ISET。
- 缓存字符: `0-9`, `.`, `-`, 空格。
- 字体先渲染到逻辑横屏临时 buffer，再预旋转到物理方向缓存。
- 预旋转阶段使用 MDMA linked-list，仅在缓存构建时运行。
- 运行时直接 `LCD_BlitRotatedRectRGB565Clean()` + DMA2D 搬缓存，不再逐像素绘制或每帧 MDMA 旋转。

普通小字体使用 tight blit 和固定槽显示；大数字和设定值仍优先走预旋转 cache。

## 编码器输入

`UF4APP/USER/setpoint_input.c` 管理 VSET/ISET 两路编码器和按键：

- TIM4: VSET 编码器。
- TIM2: ISET 编码器。
- 编码器 PUSH 切换当前编辑位，当前方向为反向循环。
- 当前按一格机械 detent 只变化 1 个单位步进，通过 `SETPOINT_ENCODER_COUNTS_PER_STEP = 2` 对硬件计数做累积折算。

VSET/ISET 不放入 5 键菜单，继续由各自编码器独立调整。

## 5 键保护/输出输入

`UF4APP/USER/panel_keys.c` 管理 `KEY_L/KEY_R/KEY_UP/KEY_DN/KEY_M`：

```text
选择顺序: OTP -> OVP -> OCP -> OUT -> BLE

KEY_L / KEY_R:
  切换当前选中框，GUI 用整框 accent 边框高亮。

KEY_UP / KEY_DN:
  OTP/OVP/OCP: 调整保护值，支持长按连发和长按加速。
  OUT: UP=ON, DN=OFF。
  BLE: UP=ON, DN=OFF。

KEY_M 短按:
  OTP/OVP/OCP: 无动作。
  OUT: ON/OFF 切换，并产生 PANEL_APPLY_OUTPUT。
  BLE: ON/OFF 切换，并产生 PANEL_APPLY_BLE。

KEY_M 长按:
  当前预留，暂不执行动作。
```

当前默认值：

```text
OTP: 80.00 C，步进 1 C
OVP: 24.00 V，步进 0.1 V
OCP: 15.00 A，步进 0.1 A
OUT: OFF
BLE: OFF
```

`PanelKeys_TakeApplyEvent()` 已保留给后续通信写寄存器使用。

## 通信

`UF4APP/TELECOM/uf4_power_client.c` 使用 USART1 和 UF4COM 协议：

- 支持读/写 U16 寄存器。
- 支持 stream start/stop。
- UART RX 使用中断逐字节喂入 parser。
- 接收到 stream/read/write 响应后更新本地寄存器缓存和数据变化标志。

当前 5 键保护/输出菜单已产生本地状态和 apply 事件，真实写寄存器仍待接入通信层。

## 性能优化记录

当前显示性能优化的结论：

- 不改 20 Hz 数据流。
- 不在运行时使用 MDMA 做整屏/矩形旋转。
- 大字体和设定值小字体预旋转缓存收益明显。
- framebuffer 改为 non-cacheable，避免对 LTDC/DMA2D 目标区做大范围 cache 维护。
- 双缓冲 stale 区域当前通过重画同步，避免 front-to-draw 补拷带来的 DMA2D 等待和总线压力。
- 小字体 tight blit 和固定槽显示可减少空白区域 blit，并避免数值位置抖动。
- DMA2D 是运行时搬运主力；MDMA 只承担缓存生成时的旋转工作。

## 后续可做

- 将 `PanelKeys_TakeApplyEvent()` 接入 UF4COM 写寄存器，真正下发 OTP/OVP/OCP/OUT/BLE。
- 将真实遥测接入 GUI 数据源后，保持 20 Hz 数据节奏，只优化绘制路径。
- 清理 `Core/Src/mdma.c` 中未参与正式路径的整屏旋转测试 buffer，释放 SDRAM scratch。
- 继续扩大预旋转缓存覆盖范围，但应优先缓存高频、尺寸大、背景固定的字体。
