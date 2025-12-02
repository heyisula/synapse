#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

enum UltrasonicPosition {
    US_FRONT = 0,
    US_BACK,
    US_LEFT,
    US_RIGHT,
    US_COUNT
};

class Ultrasonic {
private:
    uint8_t trigPins[US_COUNT];
    uint8_t echoPins[US_COUNT];
    float distances[US_COUNT];
    unsigned long lastReadTime;
    
public:
    Ultrasonic();
    void begin();
    void update();
    float getDistance(UltrasonicPosition pos);
    float measureDistance(UltrasonicPosition pos);
    bool isObstacleDetected(UltrasonicPosition pos, float threshold);
    bool isEmergencyStop();
};

#endif