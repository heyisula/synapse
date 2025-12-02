#include "light_sensor.h"
#include "../config/pins.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

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

bool LightSensor::isPathVisible() {
    int avgLight = (lightLevels[LDR_PATH_L] + lightLevels[LDR_PATH_R]) / 2;
    return avgLight >= LIGHT_MIN;
}

bool LightSensor::isCompartmentOpen() {
    int avgLight = (lightLevels[LDR_COMP_1] + lightLevels[LDR_COMP_2]) / 2;
    return avgLight > 2000;  // Threshold for compartment light change
}

bool LightSensor::isBrightEnough() {
    for(int i = 0; i < LDR_COUNT; i++) {
        if(lightLevels[i] >= LIGHT_MIN) return true;
    }
    return false;
}
