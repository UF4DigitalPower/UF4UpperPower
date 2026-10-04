# 板级设计说明

## 板卡简称

- **G4**：STM32G474 所在板卡
- **F4**：STM32F429 所在板卡
- **DAPLink**：STM32F401 所在调试器板卡

## 串口连接

| 通信链路            | 设备端接口              | 对端接口                    | 用途                                  |
|-----------------|--------------------|-------------------------|-------------------------------------|
| G4 与 DAPLink    | G4 USART1：PC4/PC5  | DAPLink USART1：PA9/PA10 | PC 通过 `Virtual COM(target)` 与 G4 通信 |
| F4 与 DAPLink    | F4 USART1：PA9/PA10 | DAPLink USART6：PC6/PC7  | PC 通过 `Virtual COM(target)` 与 F4 通信 |
| G4 与 F4         | G4 USART2：PA2/PA3  | F4 USART6：PC6/PC7       | G4 与 F4 板间通信                        |
| ESP32 与 DAPLink | ESP32 下载串口         | DAPLink USART2：PA2/PA3  | PC 通过 `ESP32 COM(target)` 下载程序或通信   |

上述引脚按 `TX/RX` 顺序列出，板间连接时 TX 与对端 RX 交叉连接。

`Virtual COM(target)` 只连接当前由 `uf4-target` 选中的目标：

- 选择 `g474` 时，连接 DAPLink USART1 与 G4 USART1。
- 选择 `f429` 时，连接 DAPLink USART6 与 F4 USART1。
- DAPLink 上电后默认选择 F429。

目标串口使用 8 个数据位、1 个停止位、无校验（8N1）。波特率跟随 PC 串口工具的设置。

## PC 端 USB 接口

DAPLink 在 PC 端枚举为一个 USB 复合设备，提供以下功能接口：

- `CMSIS-DAP`：用于下载和调试 G4 或 F4。
- `Virtual COM(target)`：连接当前选中的 G4 或 F4 目标串口。
- `ESP32 COM(target)`：固定连接 DAPLink USART2 与 ESP32 下载串口。

切换目标时，SWD 调试接口和 `Virtual COM(target)` 会同步切换；`ESP32 COM(target)` 不受影响。

## 下载和调试

在烧录或调试 G4、F4 前，先使用 `uf4-target` 选择目标。以下命令均从 `Upper/STM32G474RBT6` 目录执行：

```powershell
.\tools\uf4-target\uf4-target.exe set f429
.\tools\uf4-target\uf4-target.exe set g474
.\tools\uf4-target\uf4-target.exe esp32-boot
.\tools\uf4-target\uf4-target.exe esp32-reset
.\tools\uf4-target\uf4-target.exe reset
.\tools\uf4-target\uf4-target.exe vtref
.\tools\uf4-target\uf4-target.exe vref-adc
.\tools\uf4-target\uf4-target.exe probe-idcode --target f429 --clock-khz 100 --repeat 3
```

### 使用 OpenOCD 烧录 F429

```powershell
.\tools\uf4-target\uf4-target.exe set f429
C:\OpenOCD\bin\openocd.exe -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg -c "adapter speed 1000; program path\to\firmware.hex verify reset exit"
```

### 使用 OpenOCD 烧录 G474

```powershell
.\tools\uf4-target\uf4-target.exe set g474
C:\OpenOCD\bin\openocd.exe -f interface/cmsis-dap.cfg -f target/stm32g4x.cfg -c "adapter speed 1000; program path\to\firmware.hex verify reset exit"
```

`reset` 命令会依次复位 G4 和 F4，执行结束后保留 F429 为当前目标。

## ESP32 下载

使用 `esptool` 下载前，先让 ESP32 进入下载模式：

```powershell
.\tools\uf4-target\uf4-target.exe esp32-boot
```

下载完成后复位 ESP32，使其恢复正常运行：

```powershell
.\tools\uf4-target\uf4-target.exe esp32-reset
```

## F429 首页实时读数

F429 通过 USART6 的 UF4COM 数据流接收 G474 遥测，首页电气读数均取自远程寄存器缓存，不使用设定值或演示数据。

| 首页字段 | 遥测来源 | 换算与显示 |
|---------|---------|-----------|
| INPUT VOLTAGE | `UF4_ID_INPUT_VOLTAGE` (`0x0A`) | 原始值 / 100，显示 V，保留两位小数 |
| INPUT CURRENT | `UF4_ID_INPUT_CURRENT` (`0x0B`) | 原始值 / 100，显示 A，保留两位小数 |
| OUTPUT VOLTAGE | `UF4_ID_OUTPUT_VOLTAGE` (`0x0C`) | 原始值 / 100，显示 V，保留两位小数 |
| OUTPUT CURRENT | `UF4_ID_OUTPUT_CURRENT` (`0x0D`) | 原始值 / 100，显示 A，保留两位小数 |
| INPUT POWER | 输入电压 × 输入电流 | 两个原始值先以 32 位无符号整数相乘，再 / 10000，显示 W，保留两位小数 |
| OUTPUT POWER | 输出电压 × 输出电流 | 两个原始值先以 32 位无符号整数相乘，再 / 10000，显示 W，保留两位小数 |

输入电压与输入功率显示在电压区域底部；输入电流与输出功率显示在电流区域底部。原有 480 × 800 布局保持不变，标题和值使用分开的固定宽度标签。

四个电压/电流字段已经包含在首页的 18 项数据流中，不需要新增功率寄存器。断链或任一来源字段尚未有效时，对应数值和依赖它的功率显示 `--`。当前 G474 电流遥测使用非负的 centi-A 值，因此计算结果是非负功率，不表示带符号的能量流向。`ENERGY` 和 `EFF` 仍显示 `--`，尚未提供累计能量或效率计算。

### 2026-10-04 首页缺显示修复记录

原首页只绘制了 `INPUT VOLTAGE`、`INPUT CURRENT`、`INPUT POWER` 和 `OUTPUT POWER` 的标题，没有对应数值标签。本次在原有底部行加入四个固定宽度数值标签，并在 `board_ui_refresh()` 中按在线状态和遥测字段有效性更新。首页的大号输出电压、电流也直接读取同一组真实遥测。

已完成的软件验证：

- `Tests/F429Home/run.ps1` 通过，覆盖真实寄存器来源、功率换算和四舍五入、有效零值、字段缺失、断链，以及全部 `uint16_t` 最大值。
- 使用实际生成的 Teko 字体及 Montserrat 回退字体检查标签文字宽度；底部新读数在 480 × 800 布局中未超出固定区域。
- ARM 构建通过；`2026-10-04 19:01:46` 生成 `cmake-build-codex/STM32F429IGT6.elf`、`.hex` 和 `.bin`。RAM 使用 153208 B / 192 KB，Flash 使用 528540 B / 1 MB。
- `git diff --check` 通过。

随后已由负责 OpenOCD 和串口的父代理完成固件刷写 `verify`、GDB 标签文本检查及首页截图 `cmake-build-codex/f429-home.png`。标签和截图显示输入电压 17.02 V、输入电流 0.05 A、输入功率 0.85 W、输出功率 0.00 W。

### 首页设定值同步与性能层修复

实机截图还发现 `VOLTAGE SET` 和 `CURRENT LIMIT` 仍显示 00.00，而 GDB 中 `state.voltage_set` 为 5、`state.current_limit` 为 1。原因是首页先于通信同步创建，原 `draw_set_row()` 只在页面创建时生成静态设定值文本。

本次保存两项设定值标签句柄，并在 `board_ui_refresh()` 中使用已同步的 `state.voltage_set` 和 `state.current_limit` 更新文本；后续远程同步、首页调节和预设应用也会使用该刷新路径。格式保持 `%05.2f V` / `%05.2f A`，例如同步到 5 V、1 A 后显示 `05.00 V`、`01.00 A`。

`User/LVGL/lv_conf.h` 的 `LV_USE_PERF_MONITOR` 已设为 0，避免右下角 FPS / CPU 层遮挡底部导航。首页测试已覆盖创建时为零、同步到 5 V / 1 A、后续设定值改变以及 70 V / 10 A 上限，并沿用实际字体宽度检查。本轮软件测试已通过。

本轮 ARM 构建通过，最终固件为 `cmake-build-codex/STM32F429IGT6.elf` / `.hex` / `.bin`，生成时间 `2026-10-04 19:21:54`。RAM 使用 153216 B / 192 KB，Flash 使用 527260 B / 1 MB。ELF 已不包含 `perf_monitor_init` 和 `perf_monitor_cb`；`git diff --check` 通过。

### 最终固件停机验证结果

`2026-10-04 19:21:54` 固件已通过 OpenOCD `verify` 并复位运行。硬件联调已使用 GDB 读取实际标签文本，并检查完整 RGB565 帧缓冲及导出的 `cmake-build-codex/f429-home-final.png`。首页设定值与同步状态一致，输入读数和功率完整显示，底部导航没有性能监视层遮挡。

| 实际标签 | 验证文本 |
|---------|---------|
| VOLTAGE SET | `05.00 V` |
| CURRENT LIMIT | `00.50 A` |
| INPUT VOLTAGE | `17.08 V` |
| INPUT CURRENT | `0.02 A` |
| INPUT POWER | `0.34 W` |
| OUTPUT POWER | `0.00 W` |

本次硬件验证范围仅为输出关闭状态下的 UI 显示、设定值同步和板间通信。使能输出后的通信失联排查及稳压运行验证仍未完成，以上结果不代表整机联调已经完成。

### GDB 读取首页标签实际文本

使用与已刷写固件一致的 `STM32F429IGT6.elf`，连接已选中 F429 的 OpenOCD GDB 服务。在首页已绘制并完成至少一次刷新后暂停 F429，再读取标签。`custom_home.c` 中的静态变量是 `lv_obj_t *`，对应实例实际为 `lv_label_t`；结构成员路径为 `((lv_label_t *)标签指针)->text`，与 `lv_label_get_text()` 返回的成员一致。

```gdb
set pagination off
monitor halt
p 'custom_home.c'::s_input_voltage
x/s ((lv_label_t *)'custom_home.c'::s_input_voltage)->text
x/s ((lv_label_t *)'custom_home.c'::s_input_current)->text
x/s ((lv_label_t *)'custom_home.c'::s_input_power)->text
x/s ((lv_label_t *)'custom_home.c'::s_output_power)->text
x/s ((lv_label_t *)'custom_home.c'::s_output_voltage)->text
x/s ((lv_label_t *)'custom_home.c'::s_output_current)->text
x/s ((lv_label_t *)'custom_home.c'::s_voltage_set)->text
x/s ((lv_label_t *)'custom_home.c'::s_current_limit)->text
monitor resume
```

先确认标签指针非空。页面尚未创建时指针可能为 0，切换到其他页面后旧标签指针也不能用于读取有效对象，应在首页读取。`x/s` 只检查内存中的实际标签文本，不证明 LCD 刷新或实体屏幕已显示；还需结合硬件观察完成核验。

`2026-10-04 19:21:54` 构建中 `text` 成员相对 `lv_label_t` 起始位置偏移为 48 字节，标签指针符号地址如下。重新构建后应以新 ELF 的类型和符号为准，优先使用上述结构成员表达式。

| 标签指针符号 | 当前符号地址 | 对应显示 |
|-------------|-------------|---------|
| `s_input_voltage` | `0x20003800` | INPUT VOLTAGE |
| `s_input_current` | `0x200037FC` | INPUT CURRENT |
| `s_input_power` | `0x200037F8` | INPUT POWER |
| `s_output_power` | `0x200037F4` | OUTPUT POWER |
| `s_output_voltage` | `0x20003808` | OUTPUT VOLTAGE |
| `s_output_current` | `0x20003804` | OUTPUT CURRENT |
| `s_voltage_set` | `0x200037F0` | VOLTAGE SET |
| `s_current_limit` | `0x200037EC` | CURRENT LIMIT |

### F429-G474 板间通信停机实测

`2026-10-04` 使用当前已刷写的 F429/G474 固件和 OpenOCD 非停机读取完成安全链路测试。F429 USART6 与 G474 USART2 之间持续收到有效 UF4COM 帧，STREAM 状态为活动，缓存包含 62 项参数和遥测；实测窗口内 `rx_rejected=0`、`ack_dropped=0`、`tx_submit_fail=0`。测试读取到输入约 `17.06 V`、输出约 `0.08 V`，F429 缓存状态为 `IDLE`、故障为 `0`。

测试脚本 `Tests/hil_f429_owner.py` 发送 F429 的安全停机命令（`OUTPUT_ENABLE=0`），连续读取 2 s 共 24 组缓存，结束时仍为 `state=IDLE`、`fault=0`、`OUTPUT_ENABLE=0`。本次未通过通信链路使能功率输出，因此不代表 5 V 带载稳压验收。
