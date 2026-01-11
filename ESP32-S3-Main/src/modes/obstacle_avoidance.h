#ifndef OBSTACLE_AVOIDANCE_H
#define OBSTACLE_AVOIDANCE_H

#include <Arduino.h>
#include "sensors/hcsr04.h"
#include "sensors/mpu6050.h"
#include "communication/uart.h"
#include "actuators/ermc1604syg.h"
#include "actuators/sfm27.h"
#include "config/thresholds.h"
#include "config/constants.h"

enum SafetyStatus {
    STATUS_CLEAR,
    STATUS_SLOW,
    STATUS_DONT_REVERSE,
    STATUS_STOP,
    STATUS_EMERGENCY
};

class ObstacleAvoidance {
private:
    UltrasonicManager* ultrasonicMgr;
    MotionTracker* motionTracker;
    UARTProtocol* uart;
    Display* display;
    Buzzer* buzzer;

    SafetyStatus currentStatus;
    uint8_t currentSpeed;
    bool isMoving;
    uint8_t stationaryCount;

    // Distance readings
    float frontDistance;
    float rearDistance;
    float leftDistance;
    float rightDistance;

    // Motion data
    float gyroX, gyroY;
    float accelX, accelY;
    float pitch, roll;


    void updateSensorReadings();
    bool checkFrontDistance();
    bool checkRearDistance();
    bool checkGyroAccelThreshold();
    bool checkIfStationary();
    void handleEmergencyStop();
    void handleRearObstacle();
    void handleGyroAccelExceeded();
    void displaySafetyStatus(SafetyStatus status);
    void updateMovementPermissions();

public:
    // For API/Data logging
    struct SensorData {
        float frontDist;
        float rearDist;
        float leftDist;
        float rightDist;
        float pitch;
        float roll;
        float forwardAccel;
        float sideAccel;
        SafetyStatus status;
        uint8_t speed;
    };
    
    SensorData lastSensorData;

    ObstacleAvoidance(UltrasonicManager* us, MotionTracker* mt, 
                      UARTProtocol* u, Display* d, Buzzer* b);
    
    void begin();
    void update();
    
    SafetyStatus getSafetyStatus();
    SensorData getSensorData();
    bool canMove();
    bool canReverse();
    uint8_t getRecommendedSpeed();
};

#endif