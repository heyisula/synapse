#ifndef HEALTH_MONITOR_H
#define HEALTH_MONITOR_H

#include <Arduino.h>

struct SystemHealth {
    bool esp32OK;
    bool sensorsOK;
    bool communicationOK;
    bool motorsOK;
    bool batteryOK;
    float batteryVoltage;
    float temperature;
    int uptimeSeconds;
    int freeMemory;
};

class HealthMonitor {
private:
    SystemHealth health;
    unsigned long startTime;
    unsigned long lastCheckTime;
    int checkInterval;
    
    // Sensor timeout tracking
    unsigned long lastSensorUpdate[10];
    int sensorTimeout;
    
    // Communication timeout
    unsigned long lastUARTResponse;
    int uartTimeout;
    
public:
    HealthMonitor();
    void begin();
    void update();
    
    SystemHealth getHealth() { return health; }
    bool isSystemHealthy();
    
    void reportSensorUpdate(int sensorId);
    void reportUARTResponse();
    void reportBatteryVoltage(float voltage);
    
    String getHealthReport();
    
private:
    void checkSensors();
    void checkCommunication();
    void checkMemory();
    void checkTemperature();
};

#endif