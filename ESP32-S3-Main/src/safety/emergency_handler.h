#ifndef EMERGENCY_HANDLER_H
#define EMERGENCY_HANDLER_H

#include <Arduino.h>

enum EmergencyType {
    EMERGENCY_NONE,
    EMERGENCY_BUTTON,
    EMERGENCY_COLLISION,
    EMERGENCY_SYSTEM_FAULT,
    EMERGENCY_BATTERY_CRITICAL,
    EMERGENCY_SENSOR_FAILURE,
    EMERGENCY_COMMUNICATION_LOSS
};

struct EmergencyEvent {
    EmergencyType type;
    String description;
    unsigned long timestamp;
    bool active;
};

class EmergencyHandler {
private:
    EmergencyEvent currentEmergency;
    EmergencyEvent history[10];
    int historyCount;
    
    bool emergencyActive;
    unsigned long emergencyStartTime;
    
    // Callbacks
    typedef void (*EmergencyCallback)(EmergencyType type);
    EmergencyCallback onEmergencyStart;
    EmergencyCallback onEmergencyEnd;
    
public:
    EmergencyHandler();
    void begin();
    void update();
    
    void triggerEmergency(EmergencyType type, String description);
    void clearEmergency();
    bool isEmergencyActive() { return emergencyActive; }
    
    EmergencyEvent getCurrentEmergency() { return currentEmergency; }
    String getEmergencyDescription();
    
    void setOnEmergencyStart(EmergencyCallback callback) { 
        onEmergencyStart = callback; 
    }
    void setOnEmergencyEnd(EmergencyCallback callback) { 
        onEmergencyEnd = callback; 
    }
    
private:
    void addToHistory(EmergencyEvent event);
    String emergencyTypeToString(EmergencyType type);
};

#endif