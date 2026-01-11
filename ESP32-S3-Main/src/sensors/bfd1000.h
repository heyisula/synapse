#ifndef BFD1000_H
#define BFD1000_H

#include <Arduino.h>

class LineFollower {
private:
    static const uint8_t SENSOR_COUNT = 5;

    uint8_t sensorPins[SENSOR_COUNT];

    int rawValues[SENSOR_COUNT];
    int minValues[SENSOR_COUNT];
    int maxValues[SENSOR_COUNT];
    int thresholds[SENSOR_COUNT];

    bool sensorStates[SENSOR_COUNT];      // true = black line detected
    int linePosition;          // -2000 to +2000
    int lastLinePosition;
    unsigned long lastReadTime;
    unsigned long lastLineSeenTime;

public:
    LineFollower();

    void begin();
    void calibrate(unsigned int duration);
    void update();

    bool isLineDetected();
    bool isJunction();
    bool isAllWhite();
    bool isAllBlack();
    bool isLineLost();

    int getLinePosition();     // high-resolution position
    int getError();            // PID-ready error
};

#endif
