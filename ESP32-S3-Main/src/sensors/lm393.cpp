#include "lm393.h"
#include "../config/pins.h"
#include <Arduino.h>

#define SENSOR_READ_INTERVAL 100

LightSensor::LightSensor() {
    ldrPins[LDR_PATH_L] = LDR_PATH_LEFT;
    ldrPins[LDR_PATH_R] = LDR_PATH_RIGHT;
    ldrPins[LDR_COMP_1] = LDR_COMPARTMENT_1;
    ldrPins[LDR_COMP_2] = LDR_COMPARTMENT_2;
    
    for(int i = 0; i < LDR_COUNT; i++) {
        lightLevels[i] = HIGH; // Default to HIGH (Light above threshold for many LM393 modules)
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
            lightLevels[i] = digitalRead(ldrPins[i]);
        }
        lastReadTime = currentTime;
    }
}

int LightSensor::getLightLevel(LDRPosition pos) {
    return lightLevels[pos];
}

int LightSensor::getPathDarkness() {
    // For LM393: 
    // LOW = Light detected (above threshold)
    // HIGH = Dark (below threshold)
    // If either sensor sees dark, we consider it "darker"
    if (lightLevels[LDR_PATH_L] == HIGH && lightLevels[LDR_PATH_R] == HIGH) return 255;
    if (lightLevels[LDR_PATH_L] == HIGH || lightLevels[LDR_PATH_R] == HIGH) return 128;
    return 0;
}

bool LightSensor::isCompartmentOpen() {
    // If either compartment sensor detects light (LOW), consider it open
    return (lightLevels[LDR_COMP_1] == LOW || lightLevels[LDR_COMP_2] == LOW);
}

int LightSensor::getPathLightLevel() {
    update();
    int darkness = getPathDarkness();
    if (darkness == 0) return 1000;    // Both sensors detect light (Bright)
    if (darkness == 128) return 500;   // Only one sensor detects light (Dim)
    return 0;                         // Both sensors detect dark (Dark)
}

int LightSensor::monitorCompartment(bool compartment_start) {
    if (compartment_start) {

        if (!isCompartmentMonitoringActive) {
            isCompartmentMonitoringActive = true;
            Serial.println("=== Compartment monitoring STARTED ===");
            lastCompartmentState = 255;
        }

        update();
        
        int currentState;
        if (isCompartmentOpen()) {
            currentState = 0; // OPEN
        } else {
            currentState = 255; // CLOSED
        }
        
        // Log state changes
        if (currentState != lastCompartmentState) {
            lastCompartmentState = currentState;
            Serial.println("┌─────────────────────────");
            Serial.print("│ Compartment: ");
            Serial.println(currentState == 0 ? "OPEN" : "CLOSED");
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