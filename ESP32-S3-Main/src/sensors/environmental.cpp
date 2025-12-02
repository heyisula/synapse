// ============================================================================
// FILE: ESP32-S3-Main/src/sensors/environmental.cpp
// ============================================================================
#include "environmental.h"
#include "../config/thresholds.h"
#include "../config/constants.h"

Environmental::Environmental()
    : dht(DHTPIN, DHTTYPE)
{
    temperature = 0.0;
    humidity = 0.0;
    lastReadTime = 0;
}

bool Environmental::begin() {
    dht.begin();
    delay(1000);
    return true;
}

void Environmental::update() {
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime >= 2000) { // 2s interval for stable readings
        float t = dht.readTemperature();
        float h = dht.readHumidity();

        // Only update if valid readings are received
        if (!isnan(t) && !isnan(h)) {
            temperature = t;
            humidity = h;
        }

        lastReadTime = currentTime;
    }
}

float Environmental::getTemperature() {
    return temperature;
}

float Environmental::getHumidity() {
    return humidity;
}

bool Environmental::isOptimalTemperature() {
    return temperature >= TEMP_MIN && temperature <= TEMP_MAX;
}

bool Environmental::isOptimalHumidity() {
    return humidity >= HUMIDITY_MIN && humidity <= HUMIDITY_MAX;
}

bool Environmental::isHighTemperature() {
    return temperature > TEMP_MAX;
}

bool Environmental::isLowTemperature() {
    return temperature < TEMP_MIN;
}
