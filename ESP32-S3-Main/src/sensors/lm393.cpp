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

int LightSensor::getPathLightLevel() {
    update();
    int adcValue = (lightLevels[LDR_PATH_L] + lightLevels[LDR_PATH_R]) / 2;

    // Replace A and B with your calibrated constants
    float A = 1000.0; // Example after calibration
    float B = -1.2;   // Example after calibration

    float lux = A * pow(adcValue, B);

    if (lux < 0) lux = 0;
    return lux;
}

int LightSensor::monitorCompartment(bool compartment_start) {
    if (compartment_start) {

        if (!isCompartmentMonitoringActive) {
            isCompartmentMonitoringActive = true;
            Serial.println("=== Compartment monitoring STARTED ===");
            lastCompartmentState = 255;
        }

        update();
        int avgLight = (lightLevels[LDR_COMP_1] + lightLevels[LDR_COMP_2]) / 2;
        
        int currentState;
        if (avgLight > COMPARTMENT_LIGHT_THRESHOLD) {
            currentState = 0;
        } else {
            currentState = 255;
        }
        
        // Log state changes
        if (currentState != lastCompartmentState) {
            lastCompartmentState = currentState;
            Serial.println("┌─────────────────────────");
            Serial.print("│ Compartment: ");
            Serial.println(currentState == 0 ? "OPEN" : "CLOSED");
            Serial.print("│ Light Level: ");
            Serial.println(avgLight);
            Serial.println("└─────────────────────────");
        }
        
        return currentState;
    } 
    else {
        // Stop monitoring
        if (isCompartmentMonitoringActive) {
            isCompartmentMonitoringActive = false;
            Serial.println("=== Compartment monitoring STOPPED ===");
        }
        lastCompartmentState = 255; // Default to closed when stopped
        return 255;
    }
}