#ifndef SPEED_CONTROL_H
#define SPEED_CONTROL_H

#include <Arduino.h>

class SpeedController {
private:
    // Current speeds
    int targetLeftSpeed;
    int targetRightSpeed;
    int currentLeftSpeed;
    int currentRightSpeed;
    
    // Acceleration limits (PWM units per update)
    int maxAcceleration;
    int maxDeceleration;
    
    // Smoothing
    unsigned long lastUpdateTime;
    float accelerationRate;
    
public:
    SpeedController();
    void begin();
    void update();
    
    void setTargetSpeed(int left, int right);
    void setAccelerationLimit(int maxAccel);
    
    int getCurrentLeftSpeed() { return currentLeftSpeed; }
    int getCurrentRightSpeed() { return currentRightSpeed; }
    
    void emergencyStop();
    
private:
    int smoothSpeed(int current, int target, float dt);
};

#endif