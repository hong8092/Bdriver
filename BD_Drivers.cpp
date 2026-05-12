#include "BD_Drivers.h"
#include "sin_table.h"
#include <string.h>

// ==================== Global Variables ====================

// Encoder Data
static volatile int32_t encoderPosition = 0;
static volatile int32_t targetPosition = 0;
static volatile int32_t lastEncoderCount = 0;
static uint16_t encoderRaw = 0;

// Current Sense Data
static int16_t phaseACurrent = 0;
static int16_t phaseBCurrent = 0;

// PWM Data
static uint16_t pwmA = 0;
static uint16_t pwmB = 0;
static bool dirA = false;
static bool dirB = false;

// Control State
static uint8_t controlMode = MODE_PULSE_DIR;

// ==================== PWM & Motor Control ====================

void initPWMOutput() {
    // Initialize Motor Direction Pins
    pinMode(MOTOR_A_HIGH, OUTPUT);
    pinMode(MOTOR_A_LOW, OUTPUT);
    pinMode(MOTOR_B_HIGH, OUTPUT);
    pinMode(MOTOR_B_LOW, OUTPUT);
    
    digitalWrite(MOTOR_A_HIGH, LOW);
    digitalWrite(MOTOR_A_LOW, LOW);
    digitalWrite(MOTOR_B_HIGH, LOW);
    digitalWrite(MOTOR_B_LOW, LOW);
    
    // Initialize PWM (TIM3 for CH32M030)
    pinMode(PWM_CHA, OUTPUT);
    pinMode(PWM_CHB, OUTPUT);
    
    // TIM3_CH1 and TIM3_CH2 PWM Output Setup
    // We'll use analogWrite or direct register access
    analogWrite(PWM_CHA, 0);
    analogWrite(PWM_CHB, 0);
}

void updatePWMOutput() {
    analogWrite(PWM_CHA, pwmA);
    analogWrite(PWM_CHB, pwmB);
    
    // Set Direction Pins
    digitalWrite(MOTOR_A_HIGH, dirA ? HIGH : LOW);
    digitalWrite(MOTOR_A_LOW, dirA ? LOW : HIGH);
    digitalWrite(MOTOR_B_HIGH, dirB ? HIGH : LOW);
    digitalWrite(MOTOR_B_LOW, dirB ? LOW : HIGH);
}

void setMotorOutput(uint16_t a_pwm, uint16_t b_pwm, bool a_dir, bool b_dir) {
    pwmA = a_pwm;
    pwmB = b_pwm;
    dirA = a_dir;
    dirB = b_dir;
}

// ==================== Encoder ====================

void initEncoder() {
#if USE_ENCODER == ENCODER_TYPE_AS5600
    Wire.begin();
    pinMode(AS5600_SDA, INPUT_PULLUP);
    pinMode(AS5600_SCL, INPUT_PULLUP);
    Wire.setClock(400000);
    
#elif USE_ENCODER == ENCODER_TYPE_MT6701
    pinMode(MT6701_A, INPUT);
    pinMode(MT6701_B, INPUT);
    pinMode(MT6701_Z, INPUT);
    // We'll use TIM1 as encoder interface
#endif
}

void updateEncoderPosition() {
#if USE_ENCODER == ENCODER_TYPE_AS5600
    encoderRaw = readAS5600();
    // Convert raw angle to position (0-4095 -> position
    encoderPosition = map(encoderRaw, 0, 4095, 0, 4095);
#elif USE_ENCODER == ENCODER_TYPE_MT6701
    // Read from TIM1 counter (encoder)
    encoderPosition = TIM1->CNT;
#endif
}

int32_t getEncoderPosition() {
    return encoderPosition;
}

float getEncoderAngle() {
    return (float)encoderPosition * 360.0f / 4096.0f;
}

uint16_t getEncoderRaw() {
    return encoderRaw;
}

// AS5600 I2C Read
uint16_t readAS5600() {
    Wire.beginTransmission(0x36);
    Wire.write(0x0E); // Angle Register
    Wire.endTransmission();
    
    Wire.requestFrom(0x36, 2);
    uint16_t angle = (Wire.read() << 8) | Wire.read();
    return angle & 0x0FFF;
}

// ==================== Current Sensing ====================

void initOPACurrentSense() {
    pinMode(CURRENT_A, INPUT);
    pinMode(CURRENT_B, INPUT);
    
    // Initialize ADC for current sensing
    analogReadResolution(12); // Set to 12-bit ADC resolution
    
    // Note: OPA configuration may be handled by Arduino core
    // For full OPA control, direct register access is needed
    // This is a simplified version using ADC directly
}

void readPhaseCurrents() {
    phaseACurrent = analogRead(CURRENT_A);
    phaseBCurrent = analogRead(CURRENT_B);
    
    // Convert to mA (calibrate based on hardware)
    phaseACurrent = (phaseACurrent - 2048); // Offset calibration
    phaseBCurrent = (phaseBCurrent - 2048);
}

int16_t getPhaseACurrent() {
    return phaseACurrent;
}

int16_t getPhaseBCurrent() {
    return phaseBCurrent;
}

// ==================== Pulse/Direction Interface ====================

void initPulseDirectionInterface() {
    pinMode(ENABLE_INPUT, INPUT);
    pinMode(DIR_INPUT, INPUT);
    pinMode(PULSE_INPUT, INPUT);
    
    // Set up TIM2 for pulse counting
    // Configure TIM2 as external clock source
    attachInterrupt(digitalPinToInterrupt(PULSE_INPUT), handlePulseInterrupt, RISING);
    
    targetPosition = 0;
}

void processPulseInput() {
    // Check direction pin for direction change
    // Position is updated in interrupt
}

void handlePulseInterrupt() {
    bool direction = digitalRead(DIR_INPUT);
    
    if (direction) {
        targetPosition++;
    } else {
        targetPosition--;
    }
}

int32_t getTargetPosition() {
    return targetPosition;
}

void setTargetPosition(int32_t pos) {
    targetPosition = pos;
}

// ==================== Utility ====================

void errorHandler(const char* msg) {
    Serial.print("Error: ");
    Serial.println(msg);
    while (1) {
        // Blink error LED
        digitalWrite(LED_ERROR, HIGH);
        delay(100);
        digitalWrite(LED_ERROR, LOW);
        delay(100);
    }
}

void printSystemStatus() {
    Serial.print("Enc: ");
    Serial.print(encoderPosition);
    Serial.print(" Target: ");
    Serial.print(targetPosition);
    Serial.print(" I_A: ");
    Serial.print(phaseACurrent);
    Serial.print(" I_B: ");
    Serial.println(phaseBCurrent);
}

// ==================== Serial Command Processing ====================

void processSerialCommand() {
    static char cmdBuffer[64];
    static uint8_t cmdIndex = 0;
    
    while (Serial.available() > 0) {
        char c = Serial.read();
        
        if (c == '\n' || c == '\r') {
            if (cmdIndex > 0) {
                cmdBuffer[cmdIndex] = '\0';
                // Process command
                if (strcmp(cmdBuffer, "status") == 0) {
                    printSystemStatus();
                } else if (strcmp(cmdBuffer, "reset") == 0) {
                    setTargetPosition(0);
                    Serial.println("Position Reset!");
                } else if (strncmp(cmdBuffer, "pos ", 4) == 0) {
                    int32_t newPos = atoi(cmdBuffer + 4);
                    setTargetPosition(newPos);
                    Serial.print("Set Position: ");
                    Serial.println(newPos);
                }
            }
            cmdIndex = 0;
        } else if (cmdIndex < 63) {
            cmdBuffer[cmdIndex++] = c;
        }
    }
}
