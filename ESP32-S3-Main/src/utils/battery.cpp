#include "battery.h"
#include "../config/pins.h"

Battery::Battery() {
    _adcPin = BATTERY_ADC;
}

void Battery::begin() {
    pinMode(_adcPin, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(_adcPin, ADC_11db);
}

float Battery::readVoltage() {
    const int samples = 50;
    uint32_t sum = 0;
    
    for (int i = 0; i < samples; i++) {
        sum += analogRead(_adcPin);
        delayMicroseconds(50);
    }

    float raw = sum / (float)samples;

    float adcVoltage = (raw / 4095.0f) * 3.3f;
    float batteryVoltage = adcVoltage * ((R1 + R2) / R2);

    return batteryVoltage;
}

int Battery::readPercentage() {
    float voltage = readVoltage();

    if (voltage >= 12.6) return 100;
    if (voltage <= 9.6) return 0;

    const float voltages[] = {12.6, 12.4, 12.2, 12.0, 11.8, 11.6, 11.4, 11.2, 11.0, 10.8, 9.6};
    const int percentages[] = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0};
    const int n = sizeof(voltages)/sizeof(voltages[0]);

    for (int i = 0; i < n - 1; i++) {
        if (voltage <= voltages[i] && voltage >= voltages[i + 1]) {
            float ratio = (voltage - voltages[i + 1]) / (voltages[i] - voltages[i + 1]);
            int perc = percentages[i + 1] + (int)(ratio * (percentages[i] - percentages[i + 1]));
            return perc;
        }
    }

    return 0;
}

int Battery::getBatteryLevel() {

    int batteryLevel = readPercentage();
    batteryLevel = constrain(batteryLevel, 0, 100);
    
    return batteryLevel;
}

int Battery::getVoltageInt() {
    float voltage = readVoltage();
    
    int voltageMillivolts = (int)round(voltage * 1000.0f);
    
    return voltageMillivolts;
}

void Battery::getBatteryData(int& battery, float& voltage) {
    float voltageFloat = readVoltage();
    
    voltage = voltageFloat; // Return actual float value (e.g. 13.3)

    battery = readPercentage();  

    battery = constrain(battery, 0, 100);

    Serial.print("Battery - Level: ");
    Serial.print(battery);
    Serial.print("%, Voltage: ");
    Serial.print(voltage, 2);
    Serial.println("V");
}