#ifndef BD_CONTROL_H
#define BD_CONTROL_H

#include <Arduino.h>
#include "BD_Drivers.h"

// ==================== Control Configuration ====================

#define CURRENT_LIMIT_MA   3000  // 3A current limit
#define MAX_STEPS         4096  // Steps per full rotation
#define MICROSTEP_DIV     16    // 16 microsteps
#define CONTROL_FREQ_HZ   1000  // 1kHz control loop

// PID Configurations
#define PID_KP            2.0f
#define PID_KI            0.1f
#define PID_KD            0.01f

// ==================== Control Types ====================

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral;
    float lastError;
    float output;
} PID_Controller;

typedef struct {
    int32_t currentPosition;  // Current encoder position
    int32_t targetPosition;   // Target position from pulse input
    int32_t positionError;    // Position error
    float targetVelocity;     // Target velocity (steps/s)
    float currentVelocity;    // Current velocity
    int32_t targetCurrentA;   // Phase A target current
    int32_t targetCurrentB;   // Phase B target current
    uint16_t electricalAngle; // Electrical angle
    uint8_t stepIndex;        // Step table index
    bool enabled;             // Motor enabled
    bool direction;           // Motor direction
} MotorControlData;

// ==================== Function Declarations ====================

// Initialization
void initMotorControl();

// Main Control Loop
void runMotorControl();

// Position Control
void updatePositionControl();

// Velocity Control
void updateVelocityControl();

// Current Control
void updateCurrentControl();

// Step Generation
void generateSineStep(uint16_t electricalAngle, int16_t current);
void generateFullStep(int32_t steps);

// PID
void initPID(PID_Controller* pid, float kp, float ki, float kd);
float updatePID(PID_Controller* pid, float error, float dt);

// Electrical Angle Calculation
uint16_t getElectricalAngle(int32_t mechanicalStep);

// Utility
void enableMotor(bool enable);
void setMotorDirection(bool dir);
void emergencyStop();

#endif // BD_CONTROL_H
