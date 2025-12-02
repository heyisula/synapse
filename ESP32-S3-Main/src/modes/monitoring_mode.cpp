
#include "monitoring_mode.h"
#include "../config/thresholds.h"

MonitoringMode::MonitoringMode(HeartRateSensor* hrs, Buzzer* bz, Display* disp) {
    hrSensor = hrs;
    buzzer = bz;
    display = disp;
    state = MON_IDLE;
    historyIndex = 0;
    currentData = {0, 0, 0, false};
}

void MonitoringMode::begin() {
    state = MON_ACTIVE;
}

void MonitoringMode::update() {
    hrSensor->update();
    
    if(hrSensor->isFingerDetected()) {
        currentData.heartRate = hrSensor->getHeartRate();
        currentData.spO2 = hrSensor->getSpO2();
        currentData.timestamp = millis();
        currentData.valid = hrSensor->isValid();
        
        dataHistory[historyIndex] = currentData;
        historyIndex = (historyIndex + 1) % 10;
        
        checkVitalSigns();
    }
}

void MonitoringMode::checkVitalSigns() {
    if(!currentData.valid) {
        triggerAlert("Sensor Error");
        return;
    }
    
    if(hrSensor->isTachycardia()) {
        triggerAlert("Tachycardia");
        state = MON_ALERT;
    } else if(hrSensor->isBradycardia()) {
        triggerAlert("Bradycardia");
        state = MON_ALERT;
    } else if(hrSensor->isLowOxygen()) {
        triggerAlert("Low Oxygen");
        state = MON_ALERT;
    } else {
        state = MON_ACTIVE;
    }
}

void MonitoringMode::triggerAlert(String alertType) {
    buzzer->playTone(TONE_WARNING);
    display->displayError(alertType);
}

HealthData MonitoringMode::getCurrentData() {
    return currentData;
}