#include "environmental.h"
#include "../config/pins.h"
#include "../config/thresholds.h"
#include "../config/constants.h"

#define AM2303_READ_INTERVAL 2000UL

Environmental::Environmental(uint8_t pin)
    : sensor(pin),
      temperature(NAN),
      humidity(NAN),
      lastReadTime(0)
{ }

bool Environmental::begin() {
    sensor.begin();
    delay(1000);
    return true;
}

void Environmental::update() {
    unsigned long now = millis();
    if (now - lastReadTime < AM2303_READ_INTERVAL) {
        return;
    }
    lastReadTime = now;

    // Read sensor
    int status = sensor.read();

    if (status == 0) {
        temperature = sensor.get_Temperature();
        humidity    = sensor.get_Humidity();
    } else {
        // Optionally log errors; reset readings on failure
        Serial.print("AM2302 read failed: ");
        Serial.println(AM2302::AM2302_Sensor::get_sensorState(status));
        temperature = NAN;
        humidity    = NAN;
    }
}

float Environmental::getTemperature() {
    return temperature;
}

float Environmental::getHumidity() {
    return humidity;
}

bool Environmental::isOptimalTemperature() {
    return !isnan(temperature) &&
           temperature >= TEMP_MIN &&
           temperature <= TEMP_MAX;
}

bool Environmental::isOptimalHumidity() {
    return !isnan(humidity) &&
           humidity >= HUMIDITY_MIN &&
           humidity <= HUMIDITY_MAX;
}

bool Environmental::isHighTemperature() {
    return !isnan(temperature) && temperature > TEMP_MAX;
}

bool Environmental::isLowTemperature() {
    return !isnan(temperature) && temperature < TEMP_MIN;
}
