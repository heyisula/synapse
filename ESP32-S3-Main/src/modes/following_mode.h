#ifndef FOLLOWING_MODE_H
#define FOLLOWING_MODE_H

#include <Arduino.h>
#include "../sensors/ultrasonic.h"
#include "../sensors/color_sensor.h"
#include "../communication/uart_protocol.h"

enum FollowingState {
    FOLLOW_IDLE,
    FOLLOW_SCANNING,
    FOLLOW_TRACKING,
    FOLLOW_STANDBY
};

class FollowingMode {
private:
    Ultrasonic* ultrasonic;
    ColorSensor* colorSensor;
    UARTProtocol* uart;
    FollowingState state;
    float targetDistance;
    bool targetMoving;
    unsigned long lastDetectionTime;
    int priority; // 1=Doctor, 2=Nurse, 3=Unknown
    
public:
    FollowingMode(Ultrasonic* us, ColorSensor* cs, UARTProtocol* u);
    void begin();
    void update();
    void setTargetDistance(float distance);
    bool isHumanDetected();
    void identifyStaff();
    void setPriority(int p);
    int getPriority();
};

#endif
