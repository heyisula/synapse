#ifndef AM2302_H
#define AM2302_H

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
    Environmental();

    bool begin();
    void update();
    float getTemperature();
    float getHumidity();
};

#endif