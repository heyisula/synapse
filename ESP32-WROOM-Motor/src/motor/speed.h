#ifndef SPEED_H
#define SPEED_H

#include <Arduino.h>

class SpeedController {
private:
    uint8_t minSpeed;
    uint8_t maxSpeed;
    uint8_t currentSpeedSetting;
    
    // Speed profiles
    static const uint8_t SPEED_SLOW = 30;
    static const uint8_t SPEED_MEDIUM = 60;
    static const uint8_t SPEED_FAST = 90;
    static const uint8_t SPEED_MAX = 100;

public:
    SpeedController();
    void begin();
    
    uint8_t normalizeSpeed(uint8_t speed);
    uint8_t applySpeedLimit(uint8_t requestedSpeed);
    void setSpeedLimits(uint8_t min, uint8_t max);
    
    uint8_t getMinSpeed();
    uint8_t getMaxSpeed();
    uint8_t getCurrentSpeed();
    
    uint8_t getSlowSpeed();
    uint8_t getMediumSpeed();
    uint8_t getFastSpeed();
    uint8_t getMaximumSpeed();
};

#endif