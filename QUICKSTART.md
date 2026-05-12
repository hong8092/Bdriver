# BDriver 快速开始指南

## 一、硬件准备

### 1. 芯片
- **CH32M030C8T7** - 封装：LQFP48 或 QFN48
- 供电：5~28V (高压端), 3.3V (逻辑端)

### 2. 连接方式

#### 电源连接
- VHV: 5~28V 主电源
- VDD: 3.3V 逻辑电源
- VDD8: 5~10V 预驱动电源
- GND: 共地

#### 电机连接
连接两相步进电机到：
- A相: PA0, PA1
- B相: PA2, PA3
- PWM: PB0, PB1

#### 编码器选择 (二选一)

**AS5600 磁编码器 (I2C):**
- VCC: 3.3V
- GND: GND
- SDA: PB7
- SCL: PB6

**MT6701 磁编码器 (ABZ):**
- VCC: 3.3V or 5V
- GND: GND
- A: PC6
- B: PC7
- Z: PC8 (可选)

#### 控制接口
- ENABLE: PC0 (高电平使能)
- DIR: PC1 (方向输入)
- PULSE: PC2 (脉冲输入)

#### 串口/USB
- TX: PA9
- RX: PA10
- USB: PA11 (DM), PA12 (DP)

## 二、软件安装

### 方案A: 使用 Arduino IDE

1. 下载安装 Arduino IDE: https://www.arduino.cc/en/software

2. 安装 CH32 Arduino Core:
   - 打开 File → Preferences
   - 在 Additional Boards Manager URLs 添加:
     ```
     http://arduino.airmcu.com/package_airmcu_index.json
     ```
   - 打开 Tools → Board → Boards Manager
   - 搜索 "CH32V" 并安装 "CH32V0xx / CH32M0xx Boards"

3. 选择开发板:
   - Tools → Board → "CH32V0xx / CH32M0xx Boards" → "CH32M030C8T7"
   - 选择正确的端口

4. 编译并上传
   - 打开 `Arduino/BDriver/BDriver.ino`
   - 点击上传按钮

### 方案B: 使用 PlatformIO (推荐)

1. 安装 VSCode 和 PlatformIO 插件

2. 打开项目:
   ```bash
   cd Arduino/
   code .
   ```

3. 在 PlatformIO 中:
   - 点击 "Upload" 编译并上传
   - 点击 "Monitor" 打开串口监视器

## 三、配置修改

编辑 `BD_Drivers.h`:

```cpp
// 选择编码器类型
#define USE_ENCODER  ENCODER_TYPE_MT6701  // 或 ENCODER_TYPE_AS5600

// 电流限制 (根据实际硬件调整)
#define CURRENT_LIMIT_MA   3000

// 细分倍数
#define MICROSTEP_DIV     16
```

## 四、调试与测试

### 串口通信
使用串口监视器 (波特率 115200)

可用命令:
- `status` - 查看系统状态
- `reset` - 重置位置
- `pos 1000` - 设置目标位置为 1000

### 功能验证
1. 连接EN/DIR/PLS信号到控制器
2. 发送脉冲，观察电机旋转
3. 发送 status 命令查看位置反馈

## 五、常见问题

### 问题1: 编译失败
- 确认已安装 CH32 Arduino Core
- 检查开发板型号是否选择正确

### 问题2: 电机不转
- 检查 ENABLE 引脚是否拉高
- 检查电源连接
- 查看串口输出的状态信息

### 问题3: 位置反馈不正确
- 检查编码器连接和供电
- 确认 `USE_ENCODER` 配置正确

## 六、资源链接

- CH32M030 数据手册: https://www.wch-ic.com/downloads/CH32M030DS0_PDF.html
- CH32 Arduino Core: https://github.com/openwch/arduino_core_ch32
- 原始 XDrive 项目: https://github.com/unlir/XDrive
