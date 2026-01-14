#ifndef ASSISTANT_MODE_H
#define ASSISTANT_MODE_H

#include <Arduino.h>
#include "sensors/tcs3200.h"
#include "sensors/hcsr04.h"
#include "communication/uart.h"
#include "actuators/ermc1604syg.h" 
#include "actuators/sfm27.h"
#include "config/thresholds.h"

enum StaffType {
    STAFF_UNKNOWN = 0,
    STAFF_DOCTOR,           // Blue uniform
    STAFF_MEDICAL_STUDENT,  // Red uniform
    STAFF_NURSE,            // Green uniform
    
};

enum FollowingState {
    ASSISTANT_STATE_IDLE = 0,
    ASSISTANT_STATE_WAITING_CARD,
    ASSISTANT_STATE_IDENTIFYING,
    ASSISTANT_STATE_IDENTIFIED_DELAY,
    ASSISTANT_STATE_WAITING_DISTANCE,
    ASSISTANT_STATE_DISTANCE_CONFIRM_DELAY,
    ASSISTANT_STATE_FOLLOWING,
    ASSISTANT_STATE_TRACKING_LOST,
    ASSISTANT_STATE_TRACKING_LOST_DELAY,
    ASSISTANT_STATE_ERROR
};

class AssistantMode {
private:
    ColorSensor* colorSensor;
    UltrasonicManager* ultrasonicMgr;
    UARTProtocol* uart;
    Display* display;
    Buzzer* buzzer;

    FollowingState currentState;
    StaffType identifiedStaff;
    
    bool humanDetected;
    bool isSystemActive;
    bool isFollowingMode;
    bool colorSensorCalibrated;
    
    int targetDistance;
    int distanceTolerance;
    
    unsigned long stateStartTime;
    unsigned long lastDisplayUpdate;
    unsigned long trackingLostTime;
    
    int centerDistance;
    int leftDistance;
    int rearDistance;
    int rightDistance;
    
    void waitForCardPunch();
    void identifyStaff();
    void waitForProperDistance();
    void followHuman();
    void handleTrackingLost();
    StaffType colorToStaffType(ColorType color);
    String staffTypeToString(StaffType staff);
    void displayCurrentState();
    void sendFollowingCommand();
    void stopRobot();

public:
    AssistantMode(ColorSensor* cs, UltrasonicManager* us, 
                       UARTProtocol* uart, Display* disp, Buzzer* buzz);
    
    void begin();
    void setFollowingMode(bool enable);
    void update();
    
    FollowingState getState() { return currentState; }
    StaffType getIdentifiedStaff() { return identifiedStaff; }
    bool isActive() { return isSystemActive; }
};

#endif