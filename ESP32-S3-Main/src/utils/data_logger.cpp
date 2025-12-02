#include "data_logger.h"

DataLogger::DataLogger() 
    : sdAvailable(false), currentLogFile(""), bufferIndex(0),
      sessionStartTime(0), lastLogTime(0), logInterval(1000) {}

void DataLogger::begin() {
    sessionStartTime = millis();
    
    // Try to initialize SD card (if available)
    // Note: SD card pins would need to be configured
    sdAvailable = false;  // Set to false for now
    
    if (!sdAvailable) {
        Serial.println("Data Logger: Using RAM buffer (SD not available)");
    } else {
        createNewLogFile();
    }
}

void DataLogger::logEvent(String eventType, String data) {
    if (millis() - lastLogTime < logInterval) {
        return;  // Rate limiting
    }
    lastLogTime = millis();
    
    String timestamp = String(millis() - sessionStartTime);
    String logLine = timestamp + "," + eventType + "," + data;
    
    Serial.print("LOG: ");
    Serial.println(logLine);
    
    if (sdAvailable && logFile) {
        logFile.println(logLine);
        logFile.flush();
    } else {
        // Store in RAM buffer
        if (bufferIndex < 50) {
            buffer[bufferIndex].timestamp = millis();
            buffer[bufferIndex].eventType = eventType;
            buffer[bufferIndex].data = data;
            bufferIndex++;
        }
    }
}

void DataLogger::logSensorData(String sensorName, float value) {
    String data = sensorName + ":" + String(value, 2);
    logEvent("SENSOR", data);
}

void DataLogger::logError(String errorMessage) {
    logEvent("ERROR", errorMessage);
}

void DataLogger::logVitalSigns(int heartRate, int spO2) {
    String data = "HR:" + String(heartRate) + ",SpO2:" + String(spO2);
    logEvent("VITALS", data);
}

void DataLogger::logPosition(float x, float y, float heading) {
    String data = "X:" + String(x, 1) + ",Y:" + String(y, 1) + 
                  ",H:" + String(heading, 1);
    logEvent("POSITION", data);
}

void DataLogger::flush() {
    if (sdAvailable && logFile) {
        logFile.flush();
    }
}

void DataLogger::createNewLogFile() {
    if (!sdAvailable) return;
    
    // Create new log file with timestamp
    currentLogFile = "/log_" + String(millis()) + ".csv";
    
    // Write header
    logFile.println("Timestamp,EventType,Data");
    logFile.flush();
    
    Serial.print("Created log file: ");
    Serial.println(currentLogFile);
}