# BDriver - CH32M030C8T7 Motor Driver

基于XDrive项目移植的CH32M030C8T7步进电机驱动器，适用于Arduino环境。

## 硬件配置

### 芯片型号
- **MCU**: CH32M030C8T7
- **核心**: 青稞 RISC-V3B (72MHz)
- **Flash**: 64KB
- **RAM**: 12KB

### 引脚定义

| 功能 | 引脚 | 说明 |
|------|------|------|
| MOTOR_A_HIGH | PA0 | A相高端驱动 |
| MOTOR_A_LOW | PA1 | A相低端驱动 |
| MOTOR_B_HIGH | PA2 | B相高端驱动 |
| MOTOR_B_LOW | PA3 | B相低端驱动 |
| PWM_CHA | PB0 | A相PWM输出 (TIM3_CH1) |
| PWM_CHB | PB1 | B相PWM输出 (TIM3_CH2) |
| CURRENT_A | PA4 | A相电流检测 (ADC_IN4) |
| CURRENT_B | PA5 | B相电流检测 (ADC_IN5) |
| AS5600_SDA | PB7 | AS5600编码器I2C数据线 |
| AS5600_SCL | PB6 | AS5600编码器I2C时钟线 |
| MT6701_A | PC6 | MT6701 A相差分输入 |
| MT6701_B | PC7 | MT6701 B相差分输入 |
| MT6701_Z | PC8 | MT6701 Z相索引脉冲 |
| PULSE_INPUT | PC2 | 脉冲输入 (TIM2_CH3) |
| DIR_INPUT | PC1 | 方向输入 |
| ENABLE_INPUT | PC0 | 使能输入 |
| UART_TX | PA9 | 串口发送 |
| UART_RX | PA10 | 串口接收 |

## 功能特性

1. **双H桥驱动** - 可驱动两相步进电机
2. **内置OPA电流检测** - 使用CH32M030内置运放
3. **双编码器支持** - AS5600 (I2C) 或 MT6701 (ABZ)
4. **脉冲/方向接口** - 传统步进电机控制方式
5. **正弦细分** - 16细分平滑驱动
6. **PID控制** - 位置/速度/电流三环控制
7. **串口通信** - 支持调试和控制命令

## 编译环境

### 前置要求

1. **Arduino IDE** - 版本 1.8.x 或 2.x
2. **CH32 Arduino Core** - WCH官方提供的Arduino核心
   - 下载地址: https://github.com/openwch/arduino_core_ch32
3. **CH32M030支持包** - 在Arduino Board Manager中安装

### 安装步骤

1. 安装Arduino IDE
2. 添加WCH Arduino Core包地址到Boards Manager:
   ```
   http://arduino.airmcu.com/package_airmcu_index.json
   ```
3. 安装 "CH32V0xx / CH32M0xx Boards"
4. 选择开发板 "CH32M030C8T7"

### 编译上传

1. 打开 `BDriver.ino`
2. 选择开发板和端口
3. 点击上传

## 串口命令

| 命令 | 功能 |
|------|------|
| status | 打印系统状态 |
| reset | 重置位置为0 |
| pos X | 设置目标位置为X |

## 控制模式

1. **PULSE_DIR** - 脉冲/方向模式 (默认)
2. **POSITION** - 位置控制模式
3. **VELOCITY** - 速度控制模式
4. **CURRENT** - 电流控制模式

## 项目结构

```
Bdriver/
├── BDriver.ino          # 主程序
├── BD_Drivers.h         # 硬件驱动头文件
├── BD_Drivers.cpp       # 硬件驱动实现
├── BD_Control.h         # 控制算法头文件
├── BD_Control.cpp       # 控制算法实现
├── sin_table.h          # 正弦查找表
├── platformio.ini       # PlatformIO配置
├── README.md            # 项目文档
├── QUICKSTART.md        # 快速开始指南
├── CH32M030_引脚定义说明.txt
└── CH32M030DS2_默认引脚说明.txt
```

## 配置说明

在 `BD_Drivers.h` 中修改配置:

```cpp
// 选择编码器类型
#define USE_ENCODER  ENCODER_TYPE_MT6701

// 电流限制 (mA)
#define CURRENT_LIMIT_MA   3000

// 细分倍数
#define MICROSTEP_DIV     16
```

## 原始项目

- GitHub: https://github.com/unlir/XDrive
- 移植说明: 从STM32F103移植到CH32M030C8T7

## 许可证

继承自XDrive项目: GPL-3.0
