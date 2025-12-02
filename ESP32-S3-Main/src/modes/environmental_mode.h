#ifndef ENVIRONMENTAL_MODE_H
#define ENVIRONMENTAL_MODE_H

#include <Arduino.h>
#include "../sensors/environmental.h"
#include "../sensors/light_sensor.h"
#include "../actuators/display.h"
#include "../actuators/buzzer.h"

enum EnvironmentalState {
    ENV_OPTIMAL,
    ENV_WARNING,
    ENV_ALERT
};

class EnvironmentalMode {
private:
    Environmental* envSensor;
    LightSensor* lightSensor;
    Display* display;
    Buzzer* buzzer;
    EnvironmentalState state;
    
public:
    EnvironmentalMode(Environmental* env, LightSensor* light, Display* disp, Buzzer* bz);
    void begin();
    void update();
    void checkConditions();
    void displayReadings();
};

#endif