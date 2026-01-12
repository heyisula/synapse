#include "speed.h"

SpeedController::SpeedController() {
    minSpeed = 0;
    maxSpeed = 100;
    currentSpeedSetting = 0;
}

void SpeedController::begin() {
    minSpeed = 0;
    maxSpeed = 100;
    currentSpeedSetting = 0;
}

uint8_t SpeedController::normalizeSpeed(uint8_t speed) {
    // Ensure speed is within valid range
    return constrain(speed, 0, 100);
}

uint8_t SpeedController::applySpeedLimit(uint8_t requestedSpeed) {
    uint8_t normalized = normalizeSpeed(requestedSpeed);
    
    if (normalized < minSpeed) {
        return minSpeed;
    }
    if (normalized > maxSpeed) {
        return maxSpeed;
    }
    
    currentSpeedSetting = normalized;
    return normalized;
}

void SpeedController::setSpeedLimits(uint8_t min, uint8_t max) {
    minSpeed = constrain(min, 0, 100);
    maxSpeed = constrain(max, minSpeed, 100);
}

uint8_t SpeedController::getMinSpeed() {
    return minSpeed;
}

uint8_t SpeedController::getMaxSpeed() {
    return maxSpeed;
}

uint8_t SpeedController::getCurrentSpeed() {
    return currentSpeedSetting;
}

uint8_t SpeedController::getSlowSpeed() {
    return SPEED_SLOW;
}

uint8_t SpeedController::getMediumSpeed() {
    return SPEED_MEDIUM;
}

uint8_t SpeedController::getFastSpeed() {
    return SPEED_FAST;
}

uint8_t SpeedController::getMaximumSpeed() {
    return SPEED_MAX;
}