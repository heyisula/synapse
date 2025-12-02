#ifndef MONITORING_MODE_H
#define MONITORING_MODE_H

#include <Arduino.h>
#include "../sensors/heartrate_sensor.h"
#include "../actuators/buzzer.h"
#include "../actuators/display.h"

enum MonitoringState {
    MON_IDLE,
    MON_ACTIVE,
    MON_ALERT
};

struct HealthData {
    int heartRate;
    int spO2;
    unsigned long timestamp;
    bool valid;
};

class MonitoringMode {
private:
    HeartRateSensor* hrSensor;
    Buzzer* buzzer;
    Display* display;
    MonitoringState state;
    HealthData currentData;
    HealthData dataHistory[10];
    int historyIndex;
    
public:
    MonitoringMode(HeartRateSensor* hrs, Buzzer* bz, Display* disp);
    void begin();
    void update();
    void checkVitalSigns();
    void triggerAlert(String alertType);
    HealthData getCurrentData();
};

#endif
