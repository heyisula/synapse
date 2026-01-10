#include "l298n.h"
#include "config/pins.h"

// ===== L298NMotor Implementation =====

L298NMotor::L298NMotor(uint8_t in1, uint8_t in2, uint8_t en, uint8_t pwmChannel)
    : in1Pin(in1), in2Pin(in2), enPin(en), PWM_CHANNEL(pwmChannel) {
    currentSpeed = 0;
    isEnabled = true;
    initialized = false;
    
    // Validate PWM channel (ESP32 supports 0-15)
    if (PWM_CHANNEL > 15) {
        Serial.print("ERROR: Invalid PWM channel ");
        Serial.print(PWM_CHANNEL);
        Serial.println(". Must be 0-15.");
        isEnabled = false;
    }
}

void L298NMotor::begin() {
    if (!isEnabled) {
        Serial.println("ERROR: Motor disabled due to invalid PWM channel");
        return;
    }
    
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    pinMode(enPin, OUTPUT);
    
    // Setup PWM and validate
    bool setupSuccess = ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    if (!setupSuccess) {
        Serial.print("ERROR: PWM setup failed for channel ");
        Serial.println(PWM_CHANNEL);
        isEnabled = false;
        initialized = false;
        return;
    }
    
    ledcAttachPin(enPin, PWM_CHANNEL);
    initialized = true;
    
    stop();
}

void L298NMotor::forward(uint8_t speed) {
    if (!isEnabled || !initialized) return;
    
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
    
    currentSpeed = constrain(speed, 0, 100);
    uint8_t pwmValue = map(currentSpeed, 0, 100, 0, 255);
    ledcWrite(PWM_CHANNEL, pwmValue);
}

void L298NMotor::backward(uint8_t speed) {
    if (!isEnabled || !initialized) return;
    
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
    
    currentSpeed = constrain(speed, 0, 100);
    uint8_t pwmValue = map(currentSpeed, 0, 100, 0, 255);
    ledcWrite(PWM_CHANNEL, pwmValue);
}

void L298NMotor::stop() {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, LOW);
    if (initialized) {
        ledcWrite(PWM_CHANNEL, 0);
    }
    currentSpeed = 0;
}

void L298NMotor::brake() {
    if (!initialized) return;
    
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, HIGH);
    ledcWrite(PWM_CHANNEL, 255);
    currentSpeed = 0;
}

uint8_t L298NMotor::getCurrentSpeed() {
    return currentSpeed;
}

void L298NMotor::setSpeed(uint8_t speed) {
    if (!isEnabled || !initialized) return;
    
    currentSpeed = constrain(speed, 0, 100);
    uint8_t pwmValue = map(currentSpeed, 0, 100, 0, 255);
    ledcWrite(PWM_CHANNEL, pwmValue);
}

// ===== L298NController Implementation =====

L298NController::L298NController() {
    // PWM Channels: 0-3 for the four motors
    leftFrontMotor = new L298NMotor(LEFT_FRONT_IN1, LEFT_FRONT_IN2, LEFT_FRONT_ENA, 0);
    leftBackMotor = new L298NMotor(LEFT_BACK_IN1, LEFT_BACK_IN2, LEFT_BACK_ENB, 1);
    rightFrontMotor = new L298NMotor(RIGHT_FRONT_IN1, RIGHT_FRONT_IN2, RIGHT_FRONT_ENA, 2);
    rightBackMotor = new L298NMotor(RIGHT_BACK_IN1, RIGHT_BACK_IN2, RIGHT_BACK_ENB, 3);
}

L298NController::~L298NController() {
    // Stop all motors before destroying
    allStop();
    
    // Delete motor objects and set pointers to nullptr
    delete leftFrontMotor;
    leftFrontMotor = nullptr;
    
    delete leftBackMotor;
    leftBackMotor = nullptr;
    
    delete rightFrontMotor;
    rightFrontMotor = nullptr;
    
    delete rightBackMotor;
    rightBackMotor = nullptr;
}

void L298NController::begin() {
    leftFrontMotor->begin();
    leftBackMotor->begin();
    rightFrontMotor->begin();
    rightBackMotor->begin();
    
    // Check if all motors initialized successfully
    if (!leftFrontMotor->isInitialized() || 
        !leftBackMotor->isInitialized() || 
        !rightFrontMotor->isInitialized() || 
        !rightBackMotor->isInitialized()) {
        Serial.println("WARNING: One or more motors failed to initialize");
    }
    
    allStop();
}

void L298NController::allForward(uint8_t speed) {
    leftFrontMotor->forward(speed);
    leftBackMotor->forward(speed);
    rightFrontMotor->forward(speed);
    rightBackMotor->forward(speed);
}

void L298NController::allBackward(uint8_t speed) {
    leftFrontMotor->backward(speed);
    leftBackMotor->backward(speed);
    rightFrontMotor->backward(speed);
    rightBackMotor->backward(speed);
}

void L298NController::allStop() {
    leftFrontMotor->stop();
    leftBackMotor->stop();
    rightFrontMotor->stop();
    rightBackMotor->stop();
}

void L298NController::allBrake() {
    leftFrontMotor->brake();
    leftBackMotor->brake();
    rightFrontMotor->brake();
    rightBackMotor->brake();
}

void L298NController::leftSideForward(uint8_t speed) {
    leftFrontMotor->forward(speed);
    leftBackMotor->forward(speed);
}

void L298NController::leftSideBackward(uint8_t speed) {
    leftFrontMotor->backward(speed);
    leftBackMotor->backward(speed);
}

void L298NController::rightSideForward(uint8_t speed) {
    rightFrontMotor->forward(speed);
    rightBackMotor->forward(speed);
}

void L298NController::rightSideBackward(uint8_t speed) {
    rightFrontMotor->backward(speed);
    rightBackMotor->backward(speed);
}

void L298NController::leftSideStop() {
    leftFrontMotor->stop();
    leftBackMotor->stop();
}

void L298NController::rightSideStop() {
    rightFrontMotor->stop();
    rightBackMotor->stop();
}