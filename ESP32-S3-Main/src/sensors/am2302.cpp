#include "sensors/am2303.h"
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
    delay(2000);
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
        float t = sensor.get_Temperature();
        float h = sensor.get_Humidity();

        // Sanity validation (very important)
        if (!isnan(t) && !isnan(h) &&
            t > -40 && t < 80 &&
            h >= 0 && h <= 100) {

            temperature = t;
            humidity = h;
            return;
        }
    }

    // If we reach here → invalid read
    temperature = NAN;
    humidity = NAN;

    Serial.print("AM2303 read error: ");
    Serial.println(AM2302::AM2302_Sensor::get_sensorState(status));
}

float Environmental::getTemperature() {
    return temperature;
}

float Environmental::getHumidity() {
    return humidity;
}
