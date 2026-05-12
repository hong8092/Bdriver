// BD_MotorControl - CH32M030C8T7 Based Motor Driver
// Based on XDrive Project (https://github.com/unlir/XDrive)
// For CH32M030C8T7 with Arduino Core

#include <Arduino.h>
#include "BD_Drivers.h"
#include "BD_Control.h"

// System Configuration
#define SYSTEM_FREQ 72000000UL
#define LOOP_INTERVAL_MS 1

// Hardware Initialization
void initHardware() {
    // Initialize PWM for Motor Control
    initPWMOutput();
    
    // Initialize Encoders (AS5600 or MT6701)
    initEncoder();
    
    // Initialize OPA for Current Sensing
    initOPACurrentSense();
    
    // Initialize Pulse/Direction Interface
    initPulseDirectionInterface();
    
    // Initialize Serial (UART) for Debug
    Serial.begin(115200);
    delay(100);
    Serial.println("BDriver CH32M030 Initialized!");
    
    // Initialize Motor Control
    initMotorControl();
}

void setup() {
    // Initialize System
    initHardware();
    
    Serial.println("System Ready!");
}

void loop() {
    static unsigned long lastLoopTime = 0;
    
    // Main Control Loop
    if (millis() - lastLoopTime >= LOOP_INTERVAL_MS) {
        lastLoopTime = millis();
        
        // Process Pulse/Direction Input
        processPulseInput();
        
        // Update Encoder Position
        updateEncoderPosition();
        
        // Read Current Sensing
        readPhaseCurrents();
        
        // Main Control Loop
        runMotorControl();
        
        // Update PWM Outputs
        updatePWMOutput();
    }
    
    // Serial Communication (Optional)
    processSerialCommand();
}

// Interrupt Handler for Pulse Input (TIM2)
void TIM2_IRQHandler() {
    handlePulseInterrupt();
}
