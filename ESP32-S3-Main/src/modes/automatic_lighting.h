#ifndef AUTOMATIC_LIGHTING_H
#define AUTOMATIC_LIGHTING_H

#include <Arduino.h>
#include "sensors/lm393.h"
#include "actuators/le0066.h"

class AutomaticLighting {
private:
    LightSensor* lightSensor;
    LEDArray* ledArray;
    
    bool isRunning;
    unsigned long lastUpdateTime;
    uint16_t updateInterval;
    
    uint8_t mapDarknessToLED(int darkness);
    
public:
    AutomaticLighting(LightSensor* sensor, LEDArray* leds);
    
    void begin();
    void start();    // Start automatic lighting
    void stop();     // Stop automatic lighting
    void update();   // Call this in loop()
};

#endif