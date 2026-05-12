#include "BD_Control.h"
#include "sin_table.h"

// ==================== Global Control Data ====================

static MotorControlData motor;
static PID_Controller posPID;
static PID_Controller velPID;
static PID_Controller currAPID;
static PID_Controller currBPID;

static uint16_t stepTable[MICROSTEP_DIV * 4]; // Full sine wave table

// ==================== Initialization ====================

void initMotorControl() {
    // Initialize PID Controllers
    initPID(&posPID, PID_KP, PID_KI, PID_KD);
    initPID(&velPID, PID_KP * 0.1f, PID_KI * 0.1f, PID_KD * 0.1f);
    initPID(&currAPID, PID_KP * 0.5f, PID_KI * 0.5f, PID_KD * 0.1f);
    initPID(&currBPID, PID_KP * 0.5f, PID_KI * 0.5f, PID_KD * 0.1f);
    
    // Initialize Motor State
    motor.currentPosition = 0;
    motor.targetPosition = 0;
    motor.targetVelocity = 0.0f;
    motor.currentVelocity = 0.0f;
    motor.electricalAngle = 0;
    motor.stepIndex = 0;
    motor.enabled = false;
    motor.direction = true;
    
    // Precompute sine table (quarter wave for efficiency)
    // Full sine is 0-4095 with 1024 steps per quarter
    for (int i = 0; i < MICROSTEP_DIV * 4; i++) {
        stepTable[i] = (uint16_t)(sin_table[i % 1024] * 1023.0f);
    }
}

// ==================== Main Control Loop ====================

void runMotorControl() {
    // Check Enable Input
    bool enableInput = digitalRead(ENABLE_INPUT);
    if (enableInput != motor.enabled) {
        enableMotor(enableInput);
    }
    
    if (!motor.enabled) {
        // Motor disabled, set outputs to low
        setMotorOutput(0, 0, false, false);
        return;
    }
    
    // Update Sensor Data
    motor.currentPosition = getEncoderPosition();
    motor.targetPosition = getTargetPosition();
    
    // Position Control
    updatePositionControl();
    
    // Velocity Control
    updateVelocityControl();
    
    // Current Control & Step Generation
    motor.electricalAngle = getElectricalAngle(motor.currentPosition);
    generateSineStep(motor.electricalAngle, (int16_t)velPID.output);
}

// ==================== Position Control ====================

void updatePositionControl() {
    motor.positionError = motor.targetPosition - motor.currentPosition;
    
    // Calculate velocity from position error
    float posError = (float)motor.positionError;
    
    // Simple P control for position (proportional to error)
    motor.targetVelocity = posPID.kp * posError;
    
    // Limit velocity
    if (motor.targetVelocity > 10000.0f) motor.targetVelocity = 10000.0f;
    if (motor.targetVelocity < -10000.0f) motor.targetVelocity = -10000.0f;
}

// ==================== Velocity Control ====================

void updateVelocityControl() {
    static int32_t lastPos = 0;
    static unsigned long lastTime = 0;
    
    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    
    if (dt > 0.0f) {
        motor.currentVelocity = (float)(motor.currentPosition - lastPos) / dt;
        lastPos = motor.currentPosition;
    }
    
    float velError = motor.targetVelocity - motor.currentVelocity;
    updatePID(&velPID, velError, dt);
    
    // Velocity output becomes current target
    motor.targetCurrentA = (int32_t)velPID.output;
    motor.targetCurrentB = motor.targetCurrentA; // Approximation
}

// ==================== Current Control ====================

void updateCurrentControl() {
    int16_t iA = getPhaseACurrent();
    int16_t iB = getPhaseBCurrent();
    
    float errA = (float)(motor.targetCurrentA - iA);
    float errB = (float)(motor.targetCurrentB - iB);
    
    float dt = 1.0f / CONTROL_FREQ_HZ;
    updatePID(&currAPID, errA, dt);
    updatePID(&currBPID, errB, dt);
}

// ==================== Step Generation ====================

void generateSineStep(uint16_t electricalAngle, int16_t current) {
    // Convert electrical angle to step index
    uint16_t index = (electricalAngle % (MICROSTEP_DIV * 4));
    
    // Get sine values for both phases
    uint16_t sinA = stepTable[index];
    uint16_t sinB = stepTable[(index + MICROSTEP_DIV) % (MICROSTEP_DIV * 4)];
    
    // Scale by current magnitude
    uint32_t currentScaled = abs(current);
    uint16_t pwmA = (sinA * currentScaled) / 4095;
    uint16_t pwmB = (sinB * currentScaled) / 4095;
    
    // Determine direction
    bool dirA = (current > 0) ? true : false;
    bool dirB = dirA;
    
    // Set motor outputs
    setMotorOutput(pwmA, pwmB, dirA, dirB);
}

void generateFullStep(int32_t steps) {
    // Full step mode (simplified for legacy systems)
    uint8_t step = (steps % 4);
    
    switch (step) {
        case 0: setMotorOutput(255, 0, true, false); break;
        case 1: setMotorOutput(0, 255, false, true); break;
        case 2: setMotorOutput(255, 0, false, true); break;
        case 3: setMotorOutput(0, 255, true, false); break;
    }
}

// ==================== Electrical Angle ====================

uint16_t getElectricalAngle(int32_t mechanicalStep) {
    // Assume 50 step motor (adjust as needed)
    return (mechanicalStep * 200) % 4096;
}

// ==================== PID Functions ====================

void initPID(PID_Controller* pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->lastError = 0.0f;
    pid->output = 0.0f;
}

float updatePID(PID_Controller* pid, float error, float dt) {
    pid->integral += error * dt;
    
    // Anti-windup
    if (pid->integral > 10000.0f) pid->integral = 10000.0f;
    if (pid->integral < -10000.0f) pid->integral = -10000.0f;
    
    float derivative = (error - pid->lastError) / dt;
    pid->lastError = error;
    
    pid->output = pid->kp * error + 
                  pid->ki * pid->integral + 
                  pid->kd * derivative;
    
    return pid->output;
}

// ==================== Utility ====================

void enableMotor(bool enable) {
    motor.enabled = enable;
    if (!enable) {
        setMotorOutput(0, 0, false, false);
    }
}

void setMotorDirection(bool dir) {
    motor.direction = dir;
}

void emergencyStop() {
    enableMotor(false);
    Serial.println("EMERGENCY STOP ACTIVATED!");
}
