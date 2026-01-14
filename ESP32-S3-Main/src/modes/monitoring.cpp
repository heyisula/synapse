#include "monitoring.h"
#include "config/thresholds.h"
#include "config/constants.h"

MonitoringSystem::MonitoringSystem(HeartRateSensor* hr, Environmental* env,
                                   LightSensor* light, Display* disp, Buzzer* buzz)
    : heartSensor(hr),
      envSensor(env),
      lightSensor(light),
      display(disp),
      buzzer(buzz),
      currentState(STATE_IDLE),
      activeAlert(ALERT_NONE),
      lastMonitoringUpdate(0),
      lastDisplayUpdate(0),
      alertStartTime(0),
      dataStoreTime(0),
      tempAlertActive(false),
      humidityAlertActive(false),
      lightAlertActive(false),
      hrAlertActive(false),
      spo2AlertActive(false)
{
    memset(&currentData, 0, sizeof(MonitoringData));
    memset(&previousData, 0, sizeof(MonitoringData));
}

bool MonitoringSystem::initialize() {
    Serial.println("=== Initializing Monitoring System ===");
    currentState = STATE_INITIALIZING;
    
    // Initialize display
    display->clear();
    display->setCursor(0, 0);
    display->print("Initializing...");
    display->setCursor(0, 1);
    display->print("Monitoring System");
    
    // Initialize heart rate sensor
    display->setCursor(0, 2);
    display->print("MAX30102...");
    if (!heartSensor->begin()) {
        display->setCursor(0, 3);
        display->print("HR Sensor FAILED!");
        buzzer->playTone(TONE_ERROR);
        currentState = STATE_ERROR;
        return false;
    }
    display->setCursor(11, 2);
    display->print("OK");
    delay(100);
    
    // Initialize environmental sensor
    display->setCursor(0, 2);
    display->print("AM2303...       ");
    if (!envSensor->begin()) {
        display->setCursor(0, 3);
        display->print("ENV Sensor FAIL!");
        buzzer->playTone(TONE_ERROR);
        currentState = STATE_ERROR;
        return false;
    }
    display->setCursor(11, 2);
    display->print("OK");
    delay(100);
    
    // Initialize light sensor
    display->setCursor(0, 2);
    display->print("LM393...        ");
    lightSensor->begin();
    display->setCursor(11, 2);
    display->print("OK");
    delay(500);
    
    // Initialization complete
    buzzer->playTone(TONE_CONFIRM);
    Serial.println("Monitoring system initialized successfully");
    
    return true;
}

void MonitoringSystem::calibrateSensors() {
    Serial.println("=== Calibrating Sensors ===");
    currentState = STATE_CALIBRATING;
    alertStartTime = millis();
    
    display->clear();
    display->setCursor(0, 0);
    display->print("Calibrating...");
    display->setCursor(0, 1);
    display->print("Please wait...");
}


void MonitoringSystem::startMonitoring() {
    Serial.println("=== Starting Continuous Monitoring ===");
    currentState = STATE_MONITORING;
    
    display->clear();
    display->setCursor(0, 0);
    display->print("Monitoring...");
    
    buzzer->singleBeep();
    
    lastMonitoringUpdate = millis();
    lastDisplayUpdate = millis();
    dataStoreTime = millis();
}

void MonitoringSystem::update() {
    if (currentState == STATE_CALIBRATING) {
        unsigned long elapsed = millis() - alertStartTime;
        int remaining = (CALIBRATION_TIME - elapsed) / 1000;
        
        if (remaining >= 0) {
           heartSensor->update();
           envSensor->update();
           lightSensor->update(); 
           
           if (millis() - lastDisplayUpdate >= 1000) {
               display->setCursor(0, 2);
               display->print("Time: ");
               display->print(remaining);
               display->print(" sec   ");
               lastDisplayUpdate = millis();
           }
           return;
        } else {
            buzzer->playTone(TONE_CONFIRM);
            Serial.println("Calibration complete");
            startMonitoring();
            return;
        }
    }

    if (currentState != STATE_MONITORING && currentState != STATE_ALERT_ACTIVE) {
        return;
    }
    
    unsigned long currentTime = millis();
    
    // Update sensors at monitoring interval
    if (currentTime - lastMonitoringUpdate >= MONITORING_INTERVAL) {
        lastMonitoringUpdate = currentTime;
        
        // Read all sensors
        readPatientVitals();
        readEnvironment();
        
        // Check for alert conditions
        checkTemperatureAlert();
        
        // Only check other conditions if temperature is normal
        if (!tempAlertActive) {
            checkHeartRateAlert();
            
            if (!hrAlertActive) {
                checkSpO2Alert();
            }
            
            checkHumidityAlert();
            
            if (!humidityAlertActive) {
                checkLightAlert();
            }
        }
        
        // Process alerts
        processAlerts();
        
        // Store data periodically
        if (currentTime - dataStoreTime >= DATA_STORE_INTERVAL) {
            storeData();
            dataStoreTime = currentTime;
        }
    }
    
    // Update display at display interval
    if (currentTime - lastDisplayUpdate >= DISPLAY_INTERVAL) {
        lastDisplayUpdate = currentTime;
        updateDisplay();
    }
}

void MonitoringSystem::readPatientVitals() {
    heartSensor->update();
    
    currentData.heartRate = heartSensor->getHeartRate();
    currentData.spO2 = heartSensor->getSpO2();
    currentData.bodyTemp = heartSensor->getTemperature();
    currentData.fingerDetected = heartSensor->isFingerDetected();
    currentData.vitalsValid = heartSensor->isValid();
    
    if (currentData.fingerDetected) {
        Serial.print("HR: ");
        Serial.print(currentData.heartRate);
        Serial.print(" bpm | SpO2: ");
        Serial.print(currentData.spO2);
        Serial.print("% | Temp: ");
        Serial.print(currentData.bodyTemp);
        Serial.println("°C");
    }
}

void MonitoringSystem::readEnvironment() {
    envSensor->update();
    lightSensor->update();
    
    currentData.ambientTemp = envSensor->getTemperature();
    currentData.humidity = envSensor->getHumidity();
    currentData.lightLevel = (lightSensor->getLightLevel(LDR_PATH_L) + 
                             lightSensor->getLightLevel(LDR_PATH_R)) / 2;
    
    currentData.environmentValid = !isnan(currentData.ambientTemp) && 
                                  !isnan(currentData.humidity);
    
    if (currentData.environmentValid) {
        Serial.print("Ambient: ");
        Serial.print(currentData.ambientTemp);
        Serial.print("°C | Humidity: ");
        Serial.print(currentData.humidity);
        Serial.print("% | Light: ");
        Serial.println(currentData.lightLevel);
    }
}

void MonitoringSystem::checkTemperatureAlert() {
    if (!currentData.environmentValid) return;
    
    if (currentData.ambientTemp < TEMP_MIN || currentData.ambientTemp > TEMP_MAX) {
        if (!tempAlertActive) {
            tempAlertActive = true;
            activeAlert = ALERT_TEMPERATURE;
            currentState = STATE_ALERT_ACTIVE;
            alertStartTime = millis();
            Serial.println("ALERT: Temperature out of range!");
        }
    } else if (currentData.ambientTemp > (TEMP_MIN + HYSTERESIS_TEMP) && 
               currentData.ambientTemp < (TEMP_MAX - HYSTERESIS_TEMP)) {
        // Only clear alert if we are safely inside the range (Hysteresis)
        tempAlertActive = false;
    }
}

void MonitoringSystem::checkHumidityAlert() {
    if (!currentData.environmentValid) return;
    
    if (currentData.humidity < HUMIDITY_MIN || currentData.humidity > HUMIDITY_MAX) {
        if (!humidityAlertActive) {
            humidityAlertActive = true;
            activeAlert = ALERT_HUMIDITY;
            currentState = STATE_ALERT_ACTIVE;
            alertStartTime = millis();
            Serial.println("ALERT: Humidity out of range!");
        }
    } else if (currentData.humidity > (HUMIDITY_MIN + HYSTERESIS_HUMIDITY) && 
               currentData.humidity < (HUMIDITY_MAX - HYSTERESIS_HUMIDITY)) {
        humidityAlertActive = false;
    }
}

void MonitoringSystem::checkLightAlert() {
    if (currentData.lightLevel < LIGHT_MIN || currentData.lightLevel > LIGHT_MAX) {
        if (!lightAlertActive) {
            lightAlertActive = true;
            activeAlert = ALERT_LIGHT;
            currentState = STATE_ALERT_ACTIVE;
            alertStartTime = millis();
            Serial.println("ALERT: Light level out of range!");
        }
    } else {
        lightAlertActive = false;
    }
}

void MonitoringSystem::checkHeartRateAlert() {
    if (!currentData.vitalsValid || !currentData.fingerDetected) return;
    
    if (currentData.heartRate < HR_MIN || currentData.heartRate > HR_MAX) {
        if (!hrAlertActive) {
            hrAlertActive = true;
            activeAlert = ALERT_HEART_RATE;
            currentState = STATE_ALERT_ACTIVE;
            alertStartTime = millis();
            Serial.println("ALERT: Heart rate abnormal!");
        }
    } else {
        hrAlertActive = false;
    }
}

void MonitoringSystem::checkSpO2Alert() {
    if (!currentData.vitalsValid || !currentData.fingerDetected) return;
    
    if (currentData.spO2 < SPO2_MIN) {
        if (!spo2AlertActive) {
            spo2AlertActive = true;
            activeAlert = ALERT_SPO2;
            currentState = STATE_ALERT_ACTIVE;
            alertStartTime = millis();
            Serial.println("ALERT: Low oxygen saturation!");
        }
    } else {
        spo2AlertActive = false;
    }
}

void MonitoringSystem::processAlerts() {
    // If any alert is active
    if (tempAlertActive || humidityAlertActive || lightAlertActive || 
        hrAlertActive || spo2AlertActive) {
        
        // Trigger unified alert system (sound + display)
        buzzer->playTone(TONE_WARNING);
        
    } else {
        // All conditions normal
        activeAlert = ALERT_NONE;
        currentState = STATE_MONITORING;
    }
}

void MonitoringSystem::updateDisplay() {
    display->clear();
    
    // Row 0: System status
    display->setCursor(0, 0);
    if (currentState == STATE_ALERT_ACTIVE) {
        display->print("** ALERT **");
    } else {
        display->print("Monitoring");
    }
    
    // Row 1: Patient vitals
    display->setCursor(0, 1);
    if (currentData.fingerDetected) {
        display->print("HR:");
        display->print(currentData.heartRate);
        display->print(" O2:");
        display->print(currentData.spO2);
        display->print("%");
    } else {
        display->print("No finger");
    }
    
    // Row 2: Environment
    display->setCursor(0, 2);
    if (currentData.environmentValid) {
        display->print("T:");
        display->print(currentData.ambientTemp, 1);
        display->print("C H:");
        display->print((int)currentData.humidity);
        display->print("%");
    } else {
        display->print("Env reading...");
    }
    
    // Row 3: Status or alert message
    display->setCursor(0, 3);
    if (activeAlert != ALERT_NONE) {
        display->print(getAlertMessage(activeAlert));
    } else {
        display->print("Status: Normal");
    }
}

String MonitoringSystem::getAlertMessage(AlertType alert) {
    switch (alert) {
        case ALERT_TEMPERATURE:
            return "Temp Alert!";
        case ALERT_HUMIDITY:
            return "Humidity Alert!";
        case ALERT_LIGHT:
            return "Light Alert!";
        case ALERT_HEART_RATE:
            return "HR Alert!";
        case ALERT_SPO2:
            return "Low O2 Alert!";
        default:
            return "Unknown Alert";
    }
}

void MonitoringSystem::storeData() {
    currentData.timestamp = millis();
    
    Serial.println("=== Storing Data ===");
    Serial.print("Timestamp: ");
    Serial.println(currentData.timestamp);
    
    // Copy current to previous
    memcpy(&previousData, &currentData, sizeof(MonitoringData));
}

void MonitoringSystem::stopMonitoring() {
    Serial.println("=== Stopping Monitoring ===");
    currentState = STATE_IDLE;
    
    display->clear();
    display->setCursor(0, 1);
    display->print("Monitoring");
    display->setCursor(0, 2);
    display->print("Stopped");
    
    buzzer->doubleBeep();
}

MonitoringState MonitoringSystem::getState() {
    return currentState;
}

bool MonitoringSystem::isMonitoring() {
    return (currentState == STATE_MONITORING || currentState == STATE_ALERT_ACTIVE);
}

bool MonitoringSystem::hasActiveAlert() {
    return (activeAlert != ALERT_NONE);
}

MonitoringData MonitoringSystem::getCurrentData() {
    return currentData;
}

int MonitoringSystem::getHeartRate() {
    return currentData.heartRate;
}

int MonitoringSystem::getSpO2() {
    return currentData.spO2;
}

float MonitoringSystem::getBodyTemperature() {
    return currentData.bodyTemp;
}

float MonitoringSystem::getAmbientTemperature() {
    return currentData.ambientTemp;
}

float MonitoringSystem::getHumidity() {
    return currentData.humidity;
}

int MonitoringSystem::getLightLevel() {
    return currentData.lightLevel;
}

void MonitoringSystem::clearAlerts() {
    tempAlertActive = false;
    humidityAlertActive = false;
    lightAlertActive = false;
    hrAlertActive = false;
    spo2AlertActive = false;
    activeAlert = ALERT_NONE;
    
    if (currentState == STATE_ALERT_ACTIVE) {
        currentState = STATE_MONITORING;
    }
    
    buzzer->singleBeep();
}

void MonitoringSystem::resetSystem() {
    stopMonitoring();
    clearAlerts();
    
    memset(&currentData, 0, sizeof(MonitoringData));
    memset(&previousData, 0, sizeof(MonitoringData));
    
    buzzer->tripleBeep();
}

void MonitoringSystem::setMonitoringInterval(unsigned long interval) {
}