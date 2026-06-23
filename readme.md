# UF4UpperPower 上位机

适用于[UF4DigitalPower](https://github.com/UF4DigitalPower/UF4UpperPower)的配套上位机工程代码

下位机链接：[UF4DigitalPower](https://github.com/UF4DigitalPower/UF4UpperPower)<BR>
PC上位机链接：[F4CP](https://github.com/UF4DigitalPower/F4CP)

本工程是基于 `STM32H743ZITx` 的数字电源上位控制与显示固件，主要负责本机人机交互、参数设定、状态显示、波形查看以及与下位电源控制板通信。

工程使用 STM32 HAL/CubeMX 初始化外设，使用 CMake + `arm-none-eabi-gcc` 构建，输出目标为 `UF4UpperPower.elf/.hex/.bin`。


### 整机外观

![整机外观](https://img.hepi.ng/v2/fFPRqKC.jpeg)

### 硬件结构

![硬件结构](https://img.hepi.ng/v2/IFPHMQk.png)

### 主界面

![主界面](https://img.hepi.ng/v2/uwfZD7j.jpeg)

### 波形界面

![波形界面](https://img.hepi.ng/v2/txQRDEL.jpeg)

## 硬件实现

### 主控与时钟

- MCU: `STM32H743ZITx`, Cortex-M7。
- 主频: `480 MHz`。
- 外部晶振: `25 MHz HSE`。
- I-Cache / D-Cache: 已开启。
- DWT cycle counter: 用于 GUI/DMA 性能统计。

### 显示硬件

- 屏幕: 2.8 寸横屏显示，GUI 逻辑分辨率 `640 x 480`。
- LCD 控制器: `ST7701`。
- 显示数据链路: `LTDC + RGB565 framebuffer`。
- 面板初始化链路: `SPI6` 发送 ST7701 初始化命令。
- 绘制加速: `DMA2D` 负责 RGB565 矩形填充、搬运和字体/图片 blit。
- 显示方向:
  - LTDC 物理 framebuffer 为 `480 x 640`。
  - GUI 逻辑坐标为横屏 `640 x 480`。
  - `UF4BSP/bsp_lcd.c` 负责逻辑横屏坐标到物理 framebuffer 的映射。

### 外部存储

- SDRAM 通过 `FMC` 接入。
- `0xC0000000` 起始区域预留给双 framebuffer。
- framebuffer 区域配置为 non-cacheable，减少 LTDC/DMA2D 目标区缓存维护。
- `.sdram` scratch/cache 区从 `0xC0400000` 开始，用于字体缓存、临时绘图缓存等。
- QSPI 外设已初始化，保留给后续外部 Flash/资源存储使用。

### 输入硬件

- 两个旋转编码器:
  - `TIM4`: VSET 电压设定编码器。
  - `TIM2`: ISET 电流设定编码器。
  - 编码器按键用于切换当前编辑位。
- 五个面板按键:
  - `KEY_L`
  - `KEY_R`
  - `KEY_UP`
  - `KEY_DN`
  - `KEY_M`
- 按键采用软件消抖、短按、长按和连发逻辑。

### 通信接口

- `USART1`: 与数字电源控制板通信，当前使用 `921600 baud`。
- `UART4`: 预留 WiFi 模块通信，当前使用 `921600 baud`。
- `UART5`: 预留 PC 调试/扩展通信，当前使用 `921600 baud`。
- 通信协议位于 `UF4COM/`，上位客户端封装位于 `UF4APP/TELECOM/`。

## 外设实现

### 初始化顺序

主要初始化入口在 `Core/Src/main.c` 和 `UF4APP/USER/app_controller.c`。

启动流程:

1. 配置 MPU。
2. 开启 I-Cache / D-Cache。
3. 初始化 GPIO、DMA、MDMA、DMA2D、FMC、I2C3、LTDC、QSPI、SPI6、TIM、UART。
4. `LCD_Init()` 初始化 LCD 抽象层和双 framebuffer。
5. `ST7701Init()` 初始化 ST7701 面板。
6. `SetpointInput_Init()` 启动 VSET/ISET 编码器。
7. `PanelKeys_Init()` 初始化五键输入状态。
8. `UF4PowerClient_Init()` 初始化 UF4COM 电源通信客户端。
9. `GUI_Init()` 显示启动 logo 并绘制初始页面。

### LTDC

- 输出格式: `RGB565`。
- framebuffer 起始地址: `0xC0000000`。
- 逻辑显示尺寸: `640 x 480`。
- 物理 buffer 与逻辑坐标方向不同，由 BSP 显示层统一处理。
- 双缓冲由 `LCD_Present()` 切换。

### DMA2D

DMA2D 是运行时绘图的主力，负责:

- 面板背景填充。
- RGB565 矩形 blit。
- 字体缓存搬运。
- 图片和 logo 分块搬运。

GUI 尽量减少整屏刷新，优先刷新变化区域，降低 DMA2D 等待和总线压力。

### MDMA

MDMA 主要用于字体缓存生成阶段的预旋转处理。

运行时不依赖 MDMA 做整屏旋转，避免在 UI 高频刷新时产生额外阻塞。

### FMC / SDRAM

SDRAM 主要用于:

- LTDC framebuffer。
- 后备 framebuffer。
- 字体缓存。
- 临时绘图 scratch buffer。

framebuffer 区域与普通缓存区分开配置，避免显示目标区频繁做 DCache clean/invalidate。

### SPI6

SPI6 用于 ST7701 初始化命令发送:

- 模式: master。
- 数据宽度: 9 bit。
- 方向: TX only。
- 速率: 约 `15 Mbit/s`。

显示像素数据不走 SPI6，像素输出由 LTDC 完成。

### I2C3

I2C3 连接 LCD 触摸相关引脚:

- `LCD_TP_SCL`
- `LCD_TP_SDA`
- `LCD_TP_INT`
- `LCD_TP_RST`

当前主要保留硬件接口，触摸交互暂未作为主要输入方式。

### TIM2 / TIM4

- `TIM2`: ISET 编码器接口。
- `TIM4`: VSET 编码器接口。
- 编码器计数在 `SetpointInput_Update()` 中转换为设定值步进。
- 为适配机械 detent，代码通过 `SETPOINT_ENCODER_COUNTS_PER_STEP` 对计数做累积折算。

### TIM6

TIM6 作为 1 ms 周期任务时基。

中断中调用:

- `SetpointInput_Update()`
- `PanelKeys_Update()`

编码器、按键消抖、长按和连发都基于这个周期执行。

### USART1 / DMA

USART1 用于 UF4COM 电源协议通信。

- 波特率: `921600`。
- RX/TX DMA 已配置。
- 当前客户端支持读寄存器、写寄存器、启动/停止数据流。
- 接收到 stream/read/write 响应后更新本地寄存器缓存。

## 软件结构

```text
Core/
  Inc, Src             CubeMX/HAL 生成的启动、时钟、外设和中断代码
Drivers/              STM32 HAL/CMSIS 驱动
UF4BSP/
  bsp_lcd.*           LCD、LTDC framebuffer、DMA2D blit、坐标映射
  bsp_font.*          Teko 字体渲染、固定槽数字、预旋转字体缓存
  bsp_st7701.*        ST7701 面板初始化
UF4APP/
  GUI/                主界面、波形界面、页面刷新和显示组件
  TELECOM/            UF4COM 电源通信客户端
  USER/               应用调度、编码器输入、五键交互
UF4COM/               UF4 通信协议子模块
script/, storage/     工程辅助文件
```

核心模块:

- `app_controller.c`: 前台调度入口，协调通信、输入和 GUI 刷新。
- `gui_screen.c`: GUI 页面切换、刷新判定、双缓冲 present。
- `gui_main_page.c`: 主界面绘制。
- `gui_wave_page.c`: 波形查看界面绘制。
- `panel_keys.c`: 五键菜单和波形页焦点逻辑。
- `setpoint_input.c`: VSET/ISET 编码器设定逻辑。
- `uf4_power_client.c`: UF4COM 协议收发、寄存器缓存和控制同步。

## 交互逻辑

### 主界面

主界面用于常规电源监控和参数设定。

显示内容:

- 顶部状态: `VIN / IIN / PIN / EFF / FAN`。
- 大数值区: `VO / IO / PO`。
- 设定区: `VSET / ISET`。
- 保护与输出: `OTP / OVP / OCP / OUT / BLE`。
- 状态区: 通信状态、电源状态、故障状态、温度等。

刷新策略:

- 顶部五个状态标签只在显示文本变化时刷新数值区域。
- `VO / IO / PO` 黑色大块只在数值变化时刷新大数字区域。
- 页面切换返回主页时强制全量重画一次，避免旧页面残留。

### VSET / ISET 编码器

VSET/ISET 不进入五键轮询菜单，由各自编码器独立控制。

编码器逻辑:

- 旋转编码器改变当前设定值。
- 按下编码器切换当前编辑位。
- 当前编辑位通过下划线高亮。
- 显示使用固定字符槽，避免 `1`、`.` 等窄字符导致数值左右跳动。

### 主界面五键

主界面五键用于保护参数、输出和页面入口。

选择顺序:

```text
OTP -> OVP -> OCP -> OUT -> BLE -> SCOPE
```

按键行为:

```text
KEY_L / KEY_R:
  切换当前选中框。

KEY_UP / KEY_DN:
  OTP/OVP/OCP: 调整保护值，支持长按连发和加速。
  OUT: UP=ON, DN=OFF。
  BLE: UP=ON, DN=OFF。

KEY_M 短按:
  OUT: ON/OFF 切换，并产生 PANEL_APPLY_OUTPUT。
  BLE: ON/OFF 切换，并产生 PANEL_APPLY_BLE。
  SCOPE: 进入波形界面。

KEY_M 长按:
  主界面预留。
```

### 波形界面

波形界面用于查看实时数据趋势，重点是低耦合、低刷新压力和可交互。

顶部状态条:

```text
SCOPE | VSET | ISET | STATE | FAULT
```

底部交互菜单:

```text
CH1 | CH2 | Y1 | Y2 | TIME | TRIG | HOLD | BACK
```

波形页按键行为:

```text
KEY_L / KEY_R:
  在底部菜单项之间移动焦点。

KEY_UP / KEY_DN:
  CH1/CH2: 切换通道数据源。
  Y1/Y2: 调整对应通道纵向量程。
  TIME: 调整时基。
  TRIG: 切换触发显示模式。
  HOLD: 设置保持状态。

KEY_M 短按:
  CH1/CH2: 开关对应通道。
  Y1/Y2/TIME/TRIG: 向上调整一档。
  HOLD: RUN/HOLD 切换。
  BACK: 返回主界面。

KEY_M 长按:
  波形页中被消费但不返回主页，避免误触导致页面跳转。
```

通道能力:

- CH1/CH2 可独立选择数据源。
- 支持数据源: `VOUT / IOUT / POUT / VIN / IIN / PIN / VSET / ISET / EFF / FAN`。
- 左侧 Y1 显示 CH1 单位和量程。
- 右侧 Y2 显示 CH2 单位和量程。
- 底部 `Y1/Y2` 聚焦后可调整纵坐标缩放。

波形刷新策略:

- 波形页 GUI 刷新周期低于主界面，优先保证按键和通信稳定。
- 波形采样历史使用固定点数环形缓存。
- 当前仅比较波形页正在显示的数据源，避免无关遥测变化导致整页重画。
- VSET/ISET 顶部显示固定宽度，并保留当前编辑位高亮。
- 顶部不再重复显示 `TIME/TRIG/HOLD/BACK`，交互项统一放到底部。

## 通信逻辑

`UF4APP/TELECOM/uf4_power_client.c` 封装了上位机对下位数字电源的访问。

当前职责:

- 初始化 UF4COM parser。
- 通过 USART1 发送读/写/stream 命令。
- 接收并解析 UF4COM 帧。
- 维护本地寄存器缓存。
- 将 VSET/ISET/保护值/输出状态同步到下位机。
- 为 GUI 提供最新遥测和状态数据。

前台通信由 `AppController_RunCommunication()` 驱动，GUI 数据由 `AppController_FillGuiFromUf4()` 汇总。

## 构建

在工程根目录或指定构建目录执行:

```powershell
cmake --build E:\PROJECT_C\STM32H743\cmake-build-debug
```

构建完成后生成:

```text
cmake-build-debug/UF4UpperPower.elf
cmake-build-debug/UF4UpperPower.hex
cmake-build-debug/UF4UpperPower.bin
```

## 维护要点

- 修改 GUI 时优先复用 `gui_internal.h` 中的绘制控件。
- 高频显示数值优先使用固定字符槽，避免闪烁和跳动。
- 主界面和波形界面按键状态独立维护，避免页面间控件耦合。
- VSET/ISET 始终由编码器处理，不放入五键焦点循环。
- 波形页避免整屏高频重画，只刷新当前显示相关数据。
- DMA2D 是运行时绘图主路径，MDMA 主要用于缓存生成。
- framebuffer 区域保持 non-cacheable，普通 scratch/cache 区按 DMA 方向维护缓存。
- CubeMX 重新生成代码后，需要检查 `Core/Src` 中用户代码段和 BSP/APP 调用是否保留。
