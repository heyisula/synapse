#ifndef LM393_H
#define LM393_H

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
    bool lightLevels[LDR_COUNT];
    unsigned long lastReadTime;
    
    bool isCompartmentMonitoringActive;
    int lastCompartmentState;
public:
    LightSensor();
    void begin();
    void update();
    
    int getLightLevel(LDRPosition pos);
    int getPathDarkness();
    bool isCompartmentOpen();

    int getPathLightLevel();
    int monitorCompartment(bool compartment_start);
};

#endif