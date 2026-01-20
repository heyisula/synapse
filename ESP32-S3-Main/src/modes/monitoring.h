#ifndef MONITORING_H
#define MONITORING_H

#include <Arduino.h>
#include "sensors/max30102.h"
#include "sensors/am2302.h"
#include "sensors/lm393.h"
#include "actuators/ermc1604syg.h"
#include "actuators/sfm27.h"

// Alert states
enum AlertType {
    ALERT_NONE,
    ALERT_TEMPERATURE,
    ALERT_HUMIDITY,
    ALERT_LIGHT,
    ALERT_HEART_RATE,
    ALERT_SPO2
};

// Monitoring states
enum MonitoringState {
    STATE_IDLE,
    STATE_INITIALIZING,
    STATE_CALIBRATING,
    STATE_MONITORING,
    STATE_ALERT_ACTIVE,
    STATE_ERROR
};

struct MonitoringData {
    // Patient vitals
    int heartRate;
    int spO2;
    float bodyTemp;
    bool fingerDetected;
    
    // Environmental data
    float ambientTemp;
    float humidity;
    int lightLevel;
    
    // Status flags
    bool vitalsValid;
    bool environmentValid;
    
    // Timestamps
    unsigned long timestamp;
};

class MonitoringSystem {
private:
    // Sensors
    HeartRateSensor* heartSensor;
    Environmental* envSensor;
    LightSensor* lightSensor;
    
    // Peripherals
    Display* display;
    Buzzer* buzzer;
    
    // State management
    MonitoringState currentState;
    AlertType activeAlert;
    
    // Data storage
    MonitoringData currentData;
    MonitoringData previousData;
    
    // Timing
    unsigned long lastMonitoringUpdate;
    unsigned long lastDisplayUpdate;
    unsigned long alertStartTime;
    unsigned long dataStoreTime;
    
    // Alert tracking
    bool tempAlertActive;
    bool humidityAlertActive;
    bool lightAlertActive;
    bool hrAlertActive;
    bool spo2AlertActive;
    
    // Configuration
    const unsigned long MONITORING_INTERVAL = 200;  // 0.2 second
    const unsigned long DISPLAY_INTERVAL = 200;       // 0.2 seconds
    const unsigned long CALIBRATION_TIME = 5000;      // 5 seconds
    const unsigned long ALERT_TIMEOUT = 30000;        // 30 seconds
    const unsigned long DATA_STORE_INTERVAL = 5000;   // 5 seconds
    
    // Internal methods
    void readPatientVitals();
    void readEnvironment();
    void checkTemperatureAlert();
    void checkHumidityAlert();
    void checkLightAlert();
    void checkHeartRateAlert();
    void checkSpO2Alert();
    void processAlerts();
    void updateDisplay();
    void storeData();
    String getAlertMessage(AlertType alert);
    
public:
    MonitoringSystem(HeartRateSensor* hr, Environmental* env, 
                     LightSensor* light, Display* disp, Buzzer* buzz);
    
    // Core functions
    bool initialize();
    void calibrateSensors();
    void startMonitoring();
    void update();
    void stopMonitoring();
    
    // State management
    MonitoringState getState();
    bool isMonitoring();
    bool hasActiveAlert();
    
    // Data access
    MonitoringData getCurrentData();
    int getHeartRate();
    int getSpO2();
    float getBodyTemperature();
    float getAmbientTemperature();
    float getHumidity();
    int getLightLevel();
    
    // Manual control
    void clearAlerts();
    void resetSystem();
    void setMonitoringInterval(unsigned long interval);
};

#endif