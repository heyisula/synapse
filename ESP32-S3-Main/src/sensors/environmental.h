#ifndef ENVIRONMENTAL_H
#define ENVIRONMENTAL_H

#include <Arduino.h>
#include <AM2302-Sensor.h>
#include "../config/pins.h"


class Environmental {
private:
    AM2302::AM2302_Sensor sensor;
    float temperature;
    float humidity;
    unsigned long lastReadTime;

public:
    Environmental(uint8_t pin);

    bool begin();
    void update();
    float getTemperature();
    float getHumidity();
    bool isOptimalTemperature();
    bool isOptimalHumidity();
    bool isHighTemperature();
    bool isLowTemperature();
};

#endif