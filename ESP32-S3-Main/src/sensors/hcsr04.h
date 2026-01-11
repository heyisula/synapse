#ifndef HCSR04_H
#define HCSR04_H

#include <Arduino.h>
#include <Ultrasonic.h>  // Erick Simoes library

enum UltrasonicPosition {
    US_FRONT = 0,
    US_BACK,
    US_LEFT,
    US_RIGHT,
    US_COUNT
};

class UltrasonicManager {
private:
    Ultrasonic* sensors[US_COUNT];  // Array of library objects
    float distances[US_COUNT];
    unsigned long lastReadTime;
    float minValid[US_COUNT];
    float maxValid[US_COUNT];


public:
    UltrasonicManager();
    void begin();
    void update();
    float getDistance(UltrasonicPosition pos);
};

#endif
