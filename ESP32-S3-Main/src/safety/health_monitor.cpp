#include "health_monitor.h"

HealthMonitor::HealthMonitor() 
    : startTime(0), lastCheckTime(0), checkInterval(5000),
      sensorTimeout(10000), lastUARTResponse(0), uartTimeout(5000) {
    health.esp32OK = true;
    health.sensorsOK = true;
    health.communicationOK = true;
    health.motorsOK = true;
    health.batteryOK = true;
    health.batteryVoltage = 12.0;
    health.temperature = 25.0;
    
    for (int i = 0; i < 10; i++) {
        lastSensorUpdate[i] = 0;
    }
}

void HealthMonitor::begin() {
    startTime = millis();
    lastCheckTime = millis();
    lastUARTResponse = millis();
}

void HealthMonitor::update() {
    unsigned long now = millis();
    
    if (now - lastCheckTime < checkInterval) {
        return;
    }
    lastCheckTime = now;
    
    // Update uptime
    health.uptimeSeconds = (now - startTime) / 1000;
    
    // Check all systems
    checkSensors();
    checkCommunication();
    checkMemory();
    checkTemperature();
    
    // Check battery
    health.batteryOK = (health.batteryVoltage > 10.5);
    
    // Overall ESP32 health
    health.esp32OK = (health.temperature < 80.0 && health.freeMemory > 10000);
}

void HealthMonitor::checkSensors() {
    unsigned long now = millis();
    bool allSensorsOK = true;
    
    for (int i = 0; i < 10; i++) {
        if (lastSensorUpdate[i] > 0 && 
            (now - lastSensorUpdate[i] > sensorTimeout)) {
            allSensorsOK = false;
            Serial.print("Sensor ");
            Serial.print(i);
            Serial.println(" timeout!");
        }
    }
    
    health.sensorsOK = allSensorsOK;
}

void HealthMonitor::checkCommunication() {
    unsigned long now = millis();
    
    if (now - lastUARTResponse > uartTimeout) {
        health.communicationOK = false;
        health.motorsOK = false;  // Assume motors not OK if no comms
    } else {
        health.communicationOK = true;
        health.motorsOK = true;
    }
}

void HealthMonitor::checkMemory() {
    health.freeMemory = ESP.getFreeHeap();
    
    if (health.freeMemory < 10000) {
        Serial.println("WARNING: Low memory!");
    }
}

void HealthMonitor::checkTemperature() {
    // ESP32-S3 internal temperature sensor
    health.temperature = temperatureRead();
    
    if (health.temperature > 75.0) {
        Serial.println("WARNING: High temperature!");
    }
}

bool HealthMonitor::isSystemHealthy() {
    return (health.esp32OK && health.sensorsOK && 
            health.communicationOK && health.motorsOK && health.batteryOK);
}

void HealthMonitor::reportSensorUpdate(int sensorId) {
    if (sensorId >= 0 && sensorId < 10) {
        lastSensorUpdate[sensorId] = millis();
    }
}

void HealthMonitor::reportUARTResponse() {
    lastUARTResponse = millis();
}

void HealthMonitor::reportBatteryVoltage(float voltage) {
    health.batteryVoltage = voltage;
}

String HealthMonitor::getHealthReport() {
    String report = "\n=== System Health Report ===\n";
    report += "Uptime: " + String(health.uptimeSeconds) + " seconds\n";
    report += "ESP32: " + String(health.esp32OK ? "OK" : "FAULT") + "\n";
    report += "Sensors: " + String(health.sensorsOK ? "OK" : "FAULT") + "\n";
    report += "Communication: " + String(health.communicationOK ? "OK" : "FAULT") + "\n";
    report += "Motors: " + String(health.motorsOK ? "OK" : "FAULT") + "\n";
    report += "Battery: " + String(health.batteryVoltage, 1) + "V ";
    report += String(health.batteryOK ? "OK" : "LOW") + "\n";
    report += "Temperature: " + String(health.temperature, 1) + "°C\n";
    report += "Free Memory: " + String(health.freeMemory) + " bytes\n";
    report += "Status: " + String(isSystemHealthy() ? "HEALTHY" : "UNHEALTHY") + "\n";
    report += "===========================\n";
    
    return report;
}