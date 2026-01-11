#include "lm393.h"
#include "../config/pins.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

#define SENSOR_READ_INTERVAL 100

LightSensor::LightSensor() {
    ldrPins[LDR_PATH_L] = LDR_PATH_LEFT;
    ldrPins[LDR_PATH_R] = LDR_PATH_RIGHT;
    ldrPins[LDR_COMP_1] = LDR_COMPARTMENT_1;
    ldrPins[LDR_COMP_2] = LDR_COMPARTMENT_2;
    
    for(int i = 0; i < LDR_COUNT; i++) {
        lightLevels[i] = 0;
    }
    lastReadTime = 0;
}

void LightSensor::begin() {
    for(int i = 0; i < LDR_COUNT; i++) {
        pinMode(ldrPins[i], INPUT);
    }
}

void LightSensor::update() {
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= SENSOR_READ_INTERVAL) {
        for(int i = 0; i < LDR_COUNT; i++) {
            lightLevels[i] = analogRead(ldrPins[i]);
        }
        lastReadTime = currentTime;
    }
}

int LightSensor::getLightLevel(LDRPosition pos) {
    return lightLevels[pos];
}

int LightSensor::getPathDarkness() {
    int avgLight = (lightLevels[LDR_PATH_L] + lightLevels[LDR_PATH_R]) / 2;
    int darkness = map(avgLight, 0, 4095, 255, 0); // ESP32 ADC: 0-4095
    return constrain(darkness, 0, 255); //255 Dark, 0 Bright
}

bool LightSensor::isCompartmentOpen() {
    int avgLight = (lightLevels[LDR_COMP_1] + lightLevels[LDR_COMP_2]) / 2;
    return avgLight > COMPARTMENT_LIGHT_THRESHOLD; // define in thresholds.h
}
