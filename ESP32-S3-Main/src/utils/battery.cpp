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

    // For 4S Li-ion (16.8V max), with 100k/33k divider, max readable is ~13.3V
    // So anything >= 13.2V is considered 100% (saturated)
    if (voltage >= 13.2) return 100;
    if (voltage <= 12.0) return 0;

    // Steep discharge curve for the readable region (13.2V down to 12.0V)
    // This represents the bottom ~15-20% of the actual battery capacity
    // but we map it to 0-100% for display utility (so it doesn't just jump 100->0)
    const float voltages[] = {13.2, 13.0, 12.9, 12.8, 12.7, 12.6, 12.5, 12.4, 12.2, 12.0};
    const int percentages[] = {100,  80,   70,   60,   50,   40,   30,   20,   10,   0};
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