#ifndef BFD1000_H
#define BFD1000_H

#include <Arduino.h>

class BFD1000 {
public:
    static const uint8_t SENSOR_COUNT = 5;

    BFD1000();

    void begin();
    void autoCalibrate(uint16_t durationMs = 3000);
    void update();

    // Raw sensor readings
    int* getRawValues();

    // Line detection
    bool isBlack(uint8_t index);   // Single sensor black detection
    bool isAnyBlack();             // Any sensor sees black
    bool isAllBlack();             // All sensors see black
    bool isAllWhite();             // All sensors see white

private:
    uint8_t sensorPins[SENSOR_COUNT];

    int rawValues[SENSOR_COUNT];
    int minValues[SENSOR_COUNT];
    int maxValues[SENSOR_COUNT];
    int thresholds[SENSOR_COUNT];

    bool blackState[SENSOR_COUNT];
};

#endif
