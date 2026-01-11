#include "sensors/am2302.h"
#include "config/pins.h"
#include "config/thresholds.h"
#include "config/constants.h"


Environmental::Environmental()
    : sensor(AM2302_DATA),
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

int Environmental::getTemperatureInt() {
    update();
    
    if (isnan(temperature)) {
        Serial.println("Warning: Temperature reading invalid, returning 0");
        return 0;
    }
    int temp = (int)round(temperature);
    return temp;
}

int Environmental::getHumidityInt() {
    update();

    if (isnan(humidity)) {
        Serial.println("Warning: Humidity reading invalid, returning 0");
        return 0;
    }
    int hum = (int)round(humidity);
    
    return hum;
}

void Environmental::getEnvironmentData(int& temp, int& humidity) {
    update();
    if (isnan(temperature)) {
        temp = 0;
        Serial.println("Warning: Temperature reading invalid");
    } else {
        temp = (int)round(temperature);
    }

    if (isnan(this->humidity)) {
        humidity = 0;
        Serial.println("Warning: Humidity reading invalid");
    } else {
        humidity = (int)round(this->humidity);
    }
    
    Serial.print("Environment - Temp: ");
    Serial.print(temp);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
}
