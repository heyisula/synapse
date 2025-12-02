#include "motor_driver.h"
#include "../config/motor_pins.h"

MotorDriver::MotorDriver() 
    : motorsEnabled(true) {
    
    // Initialize motor pin configurations
    motors[MOTOR_LEFT_FRONT] = {LEFT_FRONT_IN1, LEFT_FRONT_IN2, LEFT_FRONT_ENA};
    motors[MOTOR_LEFT_BACK] = {LEFT_BACK_IN1, LEFT_BACK_IN2, LEFT_BACK_ENB};
    motors[MOTOR_RIGHT_FRONT] = {RIGHT_FRONT_IN1, RIGHT_FRONT_IN2, RIGHT_FRONT_ENA};
    motors[MOTOR_RIGHT_BACK] = {RIGHT_BACK_IN1, RIGHT_BACK_IN2, RIGHT_BACK_ENB};
    
    // Initialize PWM channels
    pwmChannels[0] = 0;
    pwmChannels[1] = 1;
    pwmChannels[2] = 2;
    pwmChannels[3] = 3;
    
    for (int i = 0; i < 4; i++) {
        currentSpeeds[i] = 0;
    }
}

void MotorDriver::begin() {
    Serial.println("Initializing Motor Driver...");
    
    setupMotorPins();
    
    // Configure PWM for each motor
    for (int i = 0; i < 4; i++) {
        ledcSetup(pwmChannels[i], PWM_FREQ, PWM_RESOLUTION);
        ledcAttachPin(motors[i].enable, pwmChannels[i]);
    }
    
    stopAll();
    
    Serial.println("Motor Driver initialized successfully");
}

void MotorDriver::setupMotorPins() {
    for (int i = 0; i < 4; i++) {
        pinMode(motors[i].in1, OUTPUT);
        pinMode(motors[i].in2, OUTPUT);
        pinMode(motors[i].enable, OUTPUT);
        
        digitalWrite(motors[i].in1, LOW);
        digitalWrite(motors[i].in2, LOW);
        digitalWrite(motors[i].enable, LOW);
    }
}

void MotorDriver::setMotor(MotorID motor, int speed) {
    if (!motorsEnabled) {
        return;
    }
    
    // Constrain speed to valid range
    speed = constrain(speed, -255, 255);
    currentSpeeds[motor] = speed;
    
    // Set direction
    if (speed > 0) {
        setMotorDirection(motor, true);  // Forward
    } else if (speed < 0) {
        setMotorDirection(motor, false);  // Backward
        speed = -speed;  // Make positive for PWM
    } else {
        // Stop
        digitalWrite(motors[motor].in1, LOW);
        digitalWrite(motors[motor].in2, LOW);
        speed = 0;
    }
    
    // Set PWM speed
    setMotorPWM(motor, speed);
}

void MotorDriver::setAllMotors(int leftSpeed, int rightSpeed) {
    setMotor(MOTOR_LEFT_FRONT, leftSpeed);
    setMotor(MOTOR_LEFT_BACK, leftSpeed);
    setMotor(MOTOR_RIGHT_FRONT, rightSpeed);
    setMotor(MOTOR_RIGHT_BACK, rightSpeed);
}

void MotorDriver::stopAll() {
    for (int i = 0; i < 4; i++) {
        setMotor((MotorID)i, 0);
    }
}

void MotorDriver::emergencyStop() {
    Serial.println("EMERGENCY STOP!");
    
    // Immediately stop all motors
    for (int i = 0; i < 4; i++) {
        digitalWrite(motors[i].in1, LOW);
        digitalWrite(motors[i].in2, LOW);
        ledcWrite(pwmChannels[i], 0);
        currentSpeeds[i] = 0;
    }
    
    motorsEnabled = false;
}

void MotorDriver::enable() {
    motorsEnabled = true;
    Serial.println("Motors enabled");
}

void MotorDriver::disable() {
    stopAll();
    motorsEnabled = false;
    Serial.println("Motors disabled");
}

int MotorDriver::getMotorSpeed(MotorID motor) {
    return currentSpeeds[motor];
}

void MotorDriver::setMotorDirection(MotorID motor, bool forward) {
    if (forward) {
        digitalWrite(motors[motor].in1, HIGH);
        digitalWrite(motors[motor].in2, LOW);
    } else {
        digitalWrite(motors[motor].in1, LOW);
        digitalWrite(motors[motor].in2, HIGH);
    }
}

void MotorDriver::setMotorPWM(MotorID motor, int pwmValue) {
    pwmValue = constrain(pwmValue, 0, 255);
    ledcWrite(pwmChannels[motor], pwmValue);
}

// ============================================================================
// 5. motor_control/movement.h
// ============================================================================
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>
#include "motor_driver.h"

enum MovementType {
    MOVE_STOP,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_TURN_LEFT,
    MOVE_TURN_RIGHT,
    MOVE_ROTATE_LEFT,
    MOVE_ROTATE_RIGHT,
    MOVE_CUSTOM
};

class MovementController {
private:
    MotorDriver* motorDriver;
    
    MovementType currentMovement;
    int currentSpeed;
    
    // Movement parameters
    int maxSpeed;
    int turnSpeedRatio;  // Percentage of speed for turning
    
public:
    MovementController();
    void begin(MotorDriver* driver = nullptr);
    
    void setMotorDriver(MotorDriver* driver) { motorDriver = driver; }
    
    // Basic movements
    void moveForward(int speed);
    void moveBackward(int speed);
    void turnLeft(int speed, int turnRatio = 50);
    void turnRight(int speed, int turnRatio = 50);
    void rotateLeft(int speed);
    void rotateRight(int speed);
    void stop();
    
    // Advanced movements
    void setSpeed(int left, int right);
    void arcTurn(int speed, float radius);
    
    // Getters
    MovementType getCurrentMovement() { return currentMovement; }
    int getCurrentSpeed() { return currentSpeed; }
    
    void setMaxSpeed(int speed) { maxSpeed = constrain(speed, 0, 255); }
    void setTurnSpeedRatio(int ratio) { turnSpeedRatio = constrain(ratio, 0, 100); }
};

#endif
