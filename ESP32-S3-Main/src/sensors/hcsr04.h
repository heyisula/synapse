#ifndef HCSR04_H
#define HCSR04_H

#include <Arduino.h>
#include <NewPing.h>
#include <SimpleKalmanFilter.h>

enum UltrasonicPosition {
    US_FRONT = 0,
    US_BACK,
    US_LEFT,
    US_RIGHT,
    US_COUNT
};

class UltrasonicManager {
private:
    NewPing* sensors[US_COUNT];
    SimpleKalmanFilter* filters[US_COUNT];

    float distances[US_COUNT];
    unsigned long lastReadTime;
    float minValid[US_COUNT];
    float maxValid[US_COUNT];

    bool isUltrasonicMonitoringActive;
    int lastCenterDistance;
    int lastLeftDistance;
    int lastRearDistance;
    int lastRightDistance;
    
    int currentSensorIndex;
    unsigned long lastSensorReadTime;
    


public:
    UltrasonicManager();
    ~UltrasonicManager();
    void begin();
    void update();
    float getDistance(UltrasonicPosition pos);

    bool monitorUltrasonic(bool ultrasonic_start, 
                          int& center, 
                          int& left, 
                          int& rear, 
                          int& right);
};

#endif
