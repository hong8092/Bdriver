#ifndef BD_DRIVERS_H
#define BD_DRIVERS_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// ==================== Pin Definitions for CH32M030C8T7 ====================

// Motor Drive Outputs (H-Bridge)
#define MOTOR_A_HIGH PA0
#define MOTOR_A_LOW  PA1
#define MOTOR_B_HIGH PA2
#define MOTOR_B_LOW  PA3

// PWM Outputs for Current Control
#define PWM_CHA      PB0    // TIM3_CH1
#define PWM_CHB      PB1    // TIM3_CH2

// OPA Current Sense Inputs (ADC)
#define CURRENT_A    PA4    // ADC_IN4
#define CURRENT_B    PA5    // ADC_IN5

// AS5600 Encoder (I2C)
#define AS5600_SDA   PB7
#define AS5600_SCL   PB6

// MT6701 Encoder (ABZ)
#define MT6701_A     PC6
#define MT6701_B     PC7
#define MT6701_Z     PC8

// Pulse/Direction Interface
#define PULSE_INPUT  PC2    // TIM2_CH3
#define DIR_INPUT    PC1
#define ENABLE_INPUT PC0

// Serial/UART
#define UART_TX      PA9
#define UART_RX      PA10

// USB (PA11/PA12) - Handled by Arduino core

// Debug LEDs (optional)
#define LED_STATUS   PB13
#define LED_ERROR    PB14

// ==================== Configuration ====================

// Select encoder type: AS5600 or MT6701
#define ENCODER_TYPE_AS5600  0
#define ENCODER_TYPE_MT6701  1
#define USE_ENCODER          ENCODER_TYPE_MT6701

// Control Modes
#define MODE_PULSE_DIR    0
#define MODE_STEP_DIR     1
#define MODE_POSITION     2
#define MODE_VELOCITY     3
#define MODE_CURRENT      4

// ==================== Function Declarations ====================

// PWM & Motor Control
void initPWMOutput();
void updatePWMOutput();
void setMotorOutput(uint16_t a_pwm, uint16_t b_pwm, bool a_dir, bool b_dir);

// Encoder
void initEncoder();
void updateEncoderPosition();
int32_t getEncoderPosition();
float getEncoderAngle();
uint16_t getEncoderRaw();
uint16_t readAS5600(); // Added

// Current Sensing (OPA + ADC)
void initOPACurrentSense();
void readPhaseCurrents();
int16_t getPhaseACurrent();
int16_t getPhaseBCurrent();

// Pulse/Direction Interface
void initPulseDirectionInterface();
void processPulseInput();
void handlePulseInterrupt();
int32_t getTargetPosition();
void setTargetPosition(int32_t pos);

// Serial Communication
void processSerialCommand();

// Utility
void errorHandler(const char* msg);
void printSystemStatus();

#endif // BD_DRIVERS_H
