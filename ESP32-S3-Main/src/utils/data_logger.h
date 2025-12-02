#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include <FS.h>

struct LogEntry {
    unsigned long timestamp;
    String eventType;
    String data;
};

class DataLogger {
private:
    bool sdAvailable;
    String currentLogFile;
    File logFile;
    
    // RAM buffer for when SD is not available
    LogEntry buffer[50];
    int bufferIndex;
    
    unsigned long sessionStartTime;
    unsigned long lastLogTime;
    int logInterval;
    
public:
    DataLogger();
    void begin();
    
    void logEvent(String eventType, String data);
    void logSensorData(String sensorName, float value);
    void logError(String errorMessage);
    void logVitalSigns(int heartRate, int spO2);
    void logPosition(float x, float y, float heading);
    
    void flush();
    void createNewLogFile();
    
    bool isSDAvailable() { return sdAvailable; }
    String getCurrentLogFile() { return currentLogFile; }
};

#endif