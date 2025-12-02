#ifndef ENVIRONMENTAL_H
#define ENVIRONMENTAL_H

#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 19       // GPIO pin connected to AM2303 OUT (3-pin sensor)
#define DHTTYPE DHT22   // Sensor type (AM2303/DHT22 compatible)

class Environmental {
private:
    DHT dht;
    float temperature;
    float humidity;
    unsigned long lastReadTime;

public:
    Environmental();
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