#ifndef MOTOR_PROTECTION_H
#define MOTOR_PROTECTION_H

#include <Arduino.h>

struct MotorStatus {
    bool leftFrontOK;
    bool leftBackOK;
    bool rightFrontOK;
    bool rightBackOK;
    bool overTemperature;
    bool overCurrent;
};

class MotorProtection {
private:
    // Current monitoring pins (if available)
    bool currentMonitoringEnabled;
    
    // Stall detection
    unsigned long lastMovementTime;
    int lastLeftSpeed;
    int lastRightSpeed;
    bool motorsStalled;
    
    // Temperature monitoring (if available)
    bool temperatureMonitoringEnabled;
    float currentTemperature;
    float maxTemperature;
    
    // Fault counters
    int stallCount;
    int faultCount;
    
    MotorStatus status;
    
public:
    MotorProtection();
    void begin();
    void update();
    
    bool isMotorOK();
    MotorStatus getStatus() { return status; }
    
    void reportSpeed(int left, int right);
    bool detectStall();
    
    void resetFaults();
    
private:
    void checkStall();
    void updateStatus();
};

#endif