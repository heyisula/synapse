#include "heartrate_sensor.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

// OPTION 1: Keep simulation but add TODO comment
HeartRateSensor::HeartRateSensor() {
    heartRate = 0;
    spO2 = 0;
    fingerDetected = false;
    lastReadTime = 0;
}

bool HeartRateSensor::begin() {
    // TODO: Initialize MAX30102
    // Requires: SparkFun MAX3010x library
    // Wire.begin() should be called before this
    
    Serial.println("⚠️  WARNING: Using simulated MAX30102 data!");
    Serial.println("   Install SparkFun MAX3010x library for real sensor");
    
    return true;
}

void HeartRateSensor::update() {
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= 1000) {
        // SIMULATED READING - For testing only
        // TODO: Replace with actual MAX30102 library calls
        fingerDetected = true;
        heartRate = 72;
        spO2 = 98;
        
        lastReadTime = currentTime;
        
        // Print warning periodically
        static unsigned long lastWarning = 0;
        if(currentTime - lastWarning > 10000) {
            Serial.println("⚠️  Simulated heart rate data");
            lastWarning = currentTime;
        }
    }
}

int HeartRateSensor::getHeartRate() {
    return heartRate;
}

int HeartRateSensor::getSpO2() {
    return spO2;
}

bool HeartRateSensor::isFingerDetected() {
    return fingerDetected;
}

bool HeartRateSensor::isValid() {
    return heartRate >= HR_MIN && heartRate <= 200 && spO2 >= 70 && spO2 <= 100;
}

bool HeartRateSensor::isTachycardia() {
    return heartRate > HR_MAX;
}

bool HeartRateSensor::isBradycardia() {
    return heartRate < HR_MIN && heartRate > 0;
}

bool HeartRateSensor::isLowOxygen() {
    return spO2 < SPO2_MIN && spO2 > 0;
}

