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
    const int samples = 20;
    uint32_t sum = 0;

    for (int i = 0; i < samples; i++) {
        sum += analogRead(_adcPin);
        delay(2);
    }

    float raw = sum / (float)samples;

    float adcVoltage = (raw / 4095.0f) * 3.3f;
    float batteryVoltage = adcVoltage * ((R1 + R2) / R2);

    return batteryVoltage;
}

int Battery::readPercentage() {
    float voltage = readVoltage();

    if (voltage <= MIN_VOLTAGE) return 0;
    if (voltage >= MAX_VOLTAGE) return 100;

    return (int)(((voltage - MIN_VOLTAGE) /
                 (MAX_VOLTAGE - MIN_VOLTAGE)) * 100.0);
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

void Battery::getBatteryData(int& battery, int& voltage) {
    float voltageFloat = readVoltage();
    
    voltage = (int)round(voltageFloat * 1000.0f);
    
    if (voltageFloat <= MIN_VOLTAGE) {
        battery = 0;
    } else if (voltageFloat >= MAX_VOLTAGE) {
        battery = 100;
    } else {
        battery = (int)(((voltageFloat - MIN_VOLTAGE) /
                        (MAX_VOLTAGE - MIN_VOLTAGE)) * 100.0f);
    }

    battery = constrain(battery, 0, 100);
    Serial.print("Battery - Level: ");
    Serial.print(battery);
    Serial.print("%, Voltage: ");
    Serial.print(voltage / 1000.0f, 2);
    Serial.println("V");
}