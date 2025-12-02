#ifndef LINE_FOLLOWER_H
#define LINE_FOLLOWER_H

#include <Arduino.h>

class LineFollower {
private:
    uint8_t sensorPins[5];
    bool sensorStates[5];
    int linePosition;
    unsigned long lastReadTime;
    
public:
    LineFollower();
    void begin();
    void update();
    bool isLineDetected();
    int getLinePosition();
    int getError();
    bool isJunction();
    bool isAllWhite();
    bool isAllBlack();
};

#endif