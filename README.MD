# F4CP / UF4DigitalPower 通信命令表

本文档是上位机 `F4CP` 与功率固件 `UF4DigitalPower` 之间 TVLCOM 通信命令的统一契约。修改通信命令、数据类型、单位或访问权限时，先改这份表，再同步：

- 上位机：`F4CP/App/Core/Session/session_power.py`
- 固件处理：`UF4DigitalPower/APP/Src/uf4_tvlcom.c`
- 示例：`UF4DigitalPower/Docs/tvl_host_example.py`

## 帧格式

```text
SOF(2) + Length(2, little-endian) + CMD(1) + SEQ(1) + Payload(N) + CRC16(2, little-endian)
```

- `SOF`: `0xAA 0x55`
- `Length`: `CMD + SEQ + Payload` 的字节数
- `SEQ`: 请求方递增序号；固件响应必须回同一个 `SEQ`
- `CRC16`: Modbus CRC16，初值 `0xFFFF`，多项式 `0xA001`，little-endian
- `Payload`: TLV 列表，格式为 `Type(1) + Length(2, little-endian) + Value(N)`

## 命令码

| 名称 | 值 | 方向 | Payload | 说明 |
| --- | ---: | --- | --- | --- |
| `ACK` | `0x00` | 固件 -> 上位机 | TLV 列表或空 | 请求成功响应。`READ` 成功时返回请求的数据；`WRITE` 成功时为空。 |
| `READ` | `0x01` | 上位机 -> 固件 | TLV 查询列表 | 每个 TLV 只填 `Type`，`Length=0`。 |
| `WRITE` | `0x02` | 上位机 -> 固件 | TLV 写入列表 | 每个 TLV 填 `Type`、长度和 little-endian 值。 |
| `REPORT` | `0x03` | 双向 | 请求空载荷；响应 TLV 列表 | 上位机发送空 payload 请求状态组，固件返回一帧 `REPORT`。 |
| `STREAM_START` | `0x04` | 上位机 -> 固件 | 周期 + TLV 查询列表 | 请求固件进入连续原始数据流模式。固件先返回 `ACK`，之后按请求通道顺序持续发送原始值。 |
| `STREAM_STOP` | `0x05` | 上位机 -> 固件 | 空 | 请求固件停止连续原始数据流模式。固件返回 `ACK`。 |
| `NACK` | `0xFF` | 固件 -> 上位机 | 空 | 命令、类型、长度、权限或载荷错误。 |

## 访问权限

| 名称 | 值 | 说明 |
| --- | ---: | --- |
| `READ` | `0x01` | 可读 |
| `WRITE` | `0x02` | 可写 |
| `READ_WRITE` | `0x03` | 可读可写 |

## 数据类型

| 名称 | Type | 长度 | 权限 | 单位 | 说明 |
| --- | ---: | ---: | --- | --- | --- |
| `INPUT_VOLTAGE` | `10` | 4 | READ | mV | 输入电压 |
| `INPUT_CURRENT` | `11` | 4 | READ | mA | 输入电流，非负值；小于 0 的测量结果不编码，按 0 上报 |
| `OUTPUT_VOLTAGE` | `12` | 4 | READ | mV | 输出电压 |
| `OUTPUT_CURRENT` | `13` | 4 | READ | mA | 输出电流，非负值；小于 0 的测量结果不编码，按 0 上报 |
| `CORE_TEMPERATURE` | `14` | 4 | READ | mC | 核心温度，摄氏度 x1000 |
| `BOARD_TEMPERATURE` | `15` | 4 | READ | mC | 板载温度，摄氏度 x1000 |
| `SET_VOLTAGE_LIMIT` | `17` | 4 | READ_WRITE | mV | 输出电压设定 |
| `SET_CURRENT_LIMIT` | `18` | 4 | READ_WRITE | mA | 输出电流设定 |
| `CC_CV_MODE` | `20` | 1 | READ | enum | `0=CC`, `1=CV` |
| `POWER_STATE` | `21` | 1 | READ_WRITE | bool | `0=OFF`, `1=ON` |
| `FAULT_STATE` | `22` | 4 | READ | bitmask | 故障位图 |
| `STATE_MACHINE_FLAG_BITS` | `23` | 1 | READ | bitmask | `0x01=INIT`, `0x02=WAIT`, `0x04=RISE`, `0x08=RUN`, `0x0F=ERR` |
| `STATE_MACHINE_STATE` | `24` | 1 | READ | enum | 当前功率拓扑：`0=NA`, `1=BUCK`, `2=BOOST`, `3=MIX` |
| `INPUT_VOLTAGE_RAW` | `25` | 4 | READ | adc | 输入电压 ADC 原始值 |
| `INPUT_CURRENT_RAW` | `26` | 4 | READ | adc | 输入电流 ADC 原始值 |
| `OUTPUT_VOLTAGE_RAW` | `27` | 4 | READ | adc | 输出电压 ADC 原始值 |
| `OUTPUT_CURRENT_RAW` | `28` | 4 | READ | adc | 输出电流 ADC 原始值 |
| `OTP_VALUE` | `29` | 4 | READ | mC | 当前过温保护参考值 |
| `OTP_SET_VALUE` | `30` | 4 | READ_WRITE | mC | 过温保护设定 |
| `OVP_VALUE` | `31` | 4 | READ | mV | 当前过压保护参考值 |
| `OVP_SET_VALUE` | `32` | 4 | READ_WRITE | mV | 过压保护设定 |
| `OCP_VALUE` | `33` | 4 | READ | mA | 当前过流保护参考值 |
| `OCP_SET_VALUE` | `34` | 4 | READ_WRITE | mA | 过流保护设定 |
| `DUTY_CMD` | `35` | 4 | READ | tick | 当前控制占空比命令 |
| `PWM_A_COMPARE` | `36` | 4 | READ | tick | 输入端 A 侧 PWM 比较值/占空比监控 |
| `PWM_D_COMPARE` | `37` | 4 | READ | tick | 输出端 D 侧 PWM 比较值/占空比监控 |
| `FAN_SPEED` | `38` | 4 | READ | permille | 风扇当前值，0 到 1000 |
| `FAN_SET_VALUE` | `39` | 4 | READ_WRITE | permille | 风扇设定值，0 到 1000 |
| `DEBUG_SNAPSHOT` | `40` | 0 | READ | virtual | 虚拟读取项。请求 `Type=40, Length=0`；响应返回控制环调试 TLV 组合。 |
| `LOOP_CURRENT_FEEDBACK` | `41` | 4 | READ | mA | 控制环输出电流反馈值 |
| `LOOP_CURRENT_REFERENCE` | `42` | 4 | READ | mA | 控制环输出电流参考/限流值 |
| `VOLTAGE_LOOP_CURRENT_REFERENCE` | `43` | 4 | READ | mV | 控制环电压参考值；名称保留历史兼容 |

## 状态组上报

上位机发送：

```text
CMD=REPORT, Payload = empty
```

固件返回：

```text
CMD=REPORT, Payload =
  TLV(INPUT_VOLTAGE)
  TLV(INPUT_CURRENT)
  TLV(OUTPUT_VOLTAGE)
  TLV(OUTPUT_CURRENT)
  TLV(CORE_TEMPERATURE)
  TLV(BOARD_TEMPERATURE)
  TLV(SET_VOLTAGE_LIMIT)
  TLV(SET_CURRENT_LIMIT)
  TLV(CC_CV_MODE)
  TLV(POWER_STATE)
  TLV(FAULT_STATE)
  TLV(STATE_MACHINE_FLAG_BITS)
  TLV(STATE_MACHINE_STATE)
  TLV(INPUT_CURRENT_RAW)
  TLV(OUTPUT_CURRENT_RAW)
  TLV(OTP_VALUE)
  TLV(OTP_SET_VALUE)
  TLV(OVP_VALUE)
  TLV(OVP_SET_VALUE)
  TLV(OCP_VALUE)
  TLV(OCP_SET_VALUE)
  TLV(DUTY_CMD)
  TLV(PWM_A_COMPARE)
  TLV(PWM_D_COMPARE)
  TLV(FAN_SPEED)
  TLV(FAN_SET_VALUE)
  TLV(LOOP_CURRENT_FEEDBACK)
  TLV(LOOP_CURRENT_REFERENCE)
  TLV(VOLTAGE_LOOP_CURRENT_REFERENCE)
```

## 连续原始数据流

为了适配持续高速采样，连续数据流只保留启动/停止命令的 TVLCOM 帧；设备进入流模式后，采样数据本身不再发送 `SOF`、`Length`、`CMD`、`SEQ`、`CRC16` 或 `EOF`。

上位机启动：

```text
CMD=STREAM_START
Payload =
  FastPeriodMs(2, little-endian)       # 当前固定为 20 ms，即 50 Hz
  FastCount(1)
  TLV(INPUT_VOLTAGE, Len=0)
  TLV(INPUT_CURRENT, Len=0)
  TLV(OUTPUT_VOLTAGE, Len=0)
  TLV(OUTPUT_CURRENT, Len=0)
  SlowPeriodMs(2, little-endian)       # 当前固定为 1000 ms，即 1 Hz
  SlowCount(1)
  TLV(CORE_TEMPERATURE, Len=0)
  TLV(BOARD_TEMPERATURE, Len=0)
  TLV(FAN_SPEED, Len=0)
  TLV(FAN_SET_VALUE, Len=0)
```

固件响应：

```text
CMD=ACK, SEQ=请求 SEQ, Payload=empty
```

随后固件先发送一个“快速组 + 慢速组”，之后每 20 ms 发送快速组；每累计 50 个快速组，再追加 1 次慢速组：

```text
# 第 1 个采样周期，以及之后每第 50 个快速周期
FastValue1 + SEP + FastValue2 + SEP + FastValue3 + SEP + FastValue4
+ SlowValue1 + SEP + SlowValue2 + SEP + SlowValue3 + SEP + SlowValue4

# 其它 20 ms 周期
FastValue1 + SEP + FastValue2 + SEP + FastValue3 + SEP + FastValue4
```

- `SEP`: `0xFE 0xED`，只用于同一组内部的相邻通道之间。快速组和慢速组之间没有额外分隔符，因为两组长度固定。
- 快速组顺序固定为：`INPUT_VOLTAGE`、`INPUT_CURRENT`、`OUTPUT_VOLTAGE`、`OUTPUT_CURRENT`。
- 慢速组顺序固定为：`CORE_TEMPERATURE`、`BOARD_TEMPERATURE`、`FAN_SPEED`、`FAN_SET_VALUE`。
- `Value(X)`: 流模式下上述 8 个通道统一发送为 `uint16 little-endian`，不带 `Type` 和 `Length`。
- 单位保持现有工程语义不变：电压 `mV`、电流 `mA`、温度 `mC`、风扇 `permille`；只是编码从原先的 4 字节压缩为 2 字节。
- 采样组之间没有额外帧头、帧尾或校验；上位机按固定频率调度和固定采样组长度切分。
- 若上位机需要写参数，建议先发送 `STREAM_STOP`，写入完成后再重新 `STREAM_START`，避免命令帧与原始数据交织。

上位机停止：

```text
CMD=STREAM_STOP, Payload=empty
```

## 常用请求

读取状态：

```text
CMD=READ, Payload = TLV(Type=10, Len=0) + TLV(Type=11, Len=0) + ...
```

写输出设定并开机：

```text
CMD=WRITE
Payload =
  TLV(Type=17, Len=4, Value=set_voltage_mv)
  TLV(Type=18, Len=4, Value=set_current_ma)
  TLV(Type=21, Len=1, Value=1)
```

读取调试快照：

```text
CMD=READ
Payload = TLV(Type=40, Len=0)
Response ACK Payload =
  TLV(Type=27, Len=4, Value=output_voltage_raw)
  TLV(Type=12, Len=4, Value=output_voltage_mv)
  TLV(Type=26, Len=4, Value=input_current_raw)
  TLV(Type=28, Len=4, Value=output_current_raw)
  TLV(Type=11, Len=4, Value=input_current_ma)
  TLV(Type=13, Len=4, Value=output_current_ma)
  TLV(Type=41, Len=4, Value=loop_current_feedback_ma)
  TLV(Type=42, Len=4, Value=loop_current_reference_ma)
  TLV(Type=43, Len=4, Value=voltage_loop_reference_mv)
```

## 同步检查清单

1. 新增 `CMD` 时，同时更新上位机 `PowerCommand`、固件 `user_tvl_cmd_t` 和本文档。
2. 新增数据 `Type` 时，同时更新上位机 `PowerDataType`、固件 `user_tvl_data_type_t`、元数据表和读写处理。
3. 改单位或长度时，同时更新 `POWER_DATA_META`、`g_user_tvl_data_descriptors` 和本文档。
4. 改写权限时，同时更新上位机 `_ensure_writable` 依赖的元数据和固件 `user_tvlcom_handle_write` 分支。
5. `REPORT` 状态组字段变更时，需同步上位机 `REPORT_STATUS_TYPES` 与固件 `report_types`。
