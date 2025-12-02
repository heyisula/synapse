#include "motor_protection.h"

MotorProtection::MotorProtection() 
    : currentMonitoringEnabled(false), lastMovementTime(0),
      lastLeftSpeed(0), lastRightSpeed(0), motorsStalled(false),
      temperatureMonitoringEnabled(false), currentTemperature(0),
      maxTemperature(80.0), stallCount(0), faultCount(0) {
    status.leftFrontOK = true;
    status.leftBackOK = true;
    status.rightFrontOK = true;
    status.rightBackOK = true;
    status.overTemperature = false;
    status.overCurrent = false;
}

void MotorProtection::begin() {
    lastMovementTime = millis();
    resetFaults();
}

void MotorProtection::update() {
    checkStall();
    updateStatus();
}

void MotorProtection::reportSpeed(int left, int right) {
    lastLeftSpeed = left;
    lastRightSpeed = right;
    
    if (left != 0 || right != 0) {
        lastMovementTime = millis();
    }
}

bool MotorProtection::detectStall() {
    // Check if motors have been commanded but not moving for too long
    unsigned long timeSinceMovement = millis() - lastMovementTime;
    
    if ((abs(lastLeftSpeed) > 50 || abs(lastRightSpeed) > 50) && 
        timeSinceMovement > 2000) {
        // Motors commanded but no movement for 2 seconds
        motorsStalled = true;
        stallCount++;
        return true;
    }
    
    return false;
}

void MotorProtection::checkStall() {
    if (detectStall()) {
        Serial.println("MOTOR_PROTECTION: Stall detected!");
        status.leftFrontOK = false;
        status.leftBackOK = false;
        status.rightFrontOK = false;
        status.rightBackOK = false;
    }
}

void MotorProtection::updateStatus() {
    // Update overall motor status
    // In a real implementation, you would read current sensors here
    
    // Reset status if motors have been idle
    if (lastLeftSpeed == 0 && lastRightSpeed == 0) {
        if (millis() - lastMovementTime > 5000) {
            // Motors idle for 5 seconds, reset stall
            motorsStalled = false;
        }
    }
}

bool MotorProtection::isMotorOK() {
    return status.leftFrontOK && status.leftBackOK && 
           status.rightFrontOK && status.rightBackOK &&
           !status.overTemperature && !status.overCurrent;
}

void MotorProtection::resetFaults() {
    status.leftFrontOK = true;
    status.leftBackOK = true;
    status.rightFrontOK = true;
    status.rightBackOK = true;
    status.overTemperature = false;
    status.overCurrent = false;
    motorsStalled = false;
    stallCount = 0;
    faultCount = 0;
}