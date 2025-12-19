#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

enum LDRPosition {
    LDR_PATH_L = 0,
    LDR_PATH_R,
    LDR_COMP_1,
    LDR_COMP_2,
    LDR_COUNT
};

class LightSensor {
private:
    uint8_t ldrPins[LDR_COUNT];
    int lightLevels[LDR_COUNT];
    unsigned long lastReadTime;
    
public:
    LightSensor();
    void begin();
    void update();
    int getLightLevel(LDRPosition pos);
    int getPathDarkness();           // returns 0-255 darkness for LEDs
    bool isCompartmentOpen();        // true if compartment open
};

#endif