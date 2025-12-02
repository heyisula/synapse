#include "speed_control.h"

SpeedController::SpeedController() 
    : targetLeftSpeed(0), targetRightSpeed(0),
      currentLeftSpeed(0), currentRightSpeed(0),
      maxAcceleration(50), maxDeceleration(100),
      lastUpdateTime(0), accelerationRate(10.0) {}

void SpeedController::begin() {
    lastUpdateTime = millis();
    currentLeftSpeed = 0;
    currentRightSpeed = 0;
    targetLeftSpeed = 0;
    targetRightSpeed = 0;
}

void SpeedController::update() {
    unsigned long now = millis();
    float dt = (now - lastUpdateTime) / 1000.0;  // seconds
    lastUpdateTime = now;
    
    // Smooth transition to target speeds
    currentLeftSpeed = smoothSpeed(currentLeftSpeed, targetLeftSpeed, dt);
    currentRightSpeed = smoothSpeed(currentRightSpeed, targetRightSpeed, dt);
}

void SpeedController::setTargetSpeed(int left, int right) {
    targetLeftSpeed = constrain(left, -255, 255);
    targetRightSpeed = constrain(right, -255, 255);
}

void SpeedController::setAccelerationLimit(int maxAccel) {
    maxAcceleration = constrain(maxAccel, 10, 200);
    maxDeceleration = maxAcceleration * 2;  // Faster deceleration
}

int SpeedController::smoothSpeed(int current, int target, float dt) {
    if (current == target) return current;
    
    int difference = target - current;
    int maxChange;
    
    if (abs(difference) < 5) {
        // Close enough, snap to target
        return target;
    }
    
    // Determine max change based on acceleration/deceleration
    if (abs(target) < abs(current)) {
        // Decelerating
        maxChange = maxDeceleration * dt / 0.01;  // Scale by time
    } else {
        // Accelerating
        maxChange = maxAcceleration * dt / 0.01;
    }
    
    // Apply change with limits
    if (difference > 0) {
        return current + min(difference, maxChange);
    } else {
        return current + max(difference, -maxChange);
    }
}

void SpeedController::emergencyStop() {
    targetLeftSpeed = 0;
    targetRightSpeed = 0;
    currentLeftSpeed = 0;
    currentRightSpeed = 0;
}