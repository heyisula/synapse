#include "emergency_handler.h"

EmergencyHandler::EmergencyHandler() 
    : historyCount(0), emergencyActive(false), 
      emergencyStartTime(0), onEmergencyStart(nullptr), 
      onEmergencyEnd(nullptr) {
    currentEmergency.type = EMERGENCY_NONE;
    currentEmergency.active = false;
}

void EmergencyHandler::begin() {
    emergencyActive = false;
}

void EmergencyHandler::update() {
    // Check if emergency has been active too long
    if (emergencyActive && 
        (millis() - emergencyStartTime > 300000)) {  // 5 minutes
        Serial.println("EMERGENCY: Long duration warning!");
    }
}

void EmergencyHandler::triggerEmergency(EmergencyType type, String description) {
    if (emergencyActive && currentEmergency.type == type) {
        return;  // Already in this emergency state
    }
    
    Serial.println("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.println("!!! EMERGENCY TRIGGERED !!!");
    Serial.print("Type: ");
    Serial.println(emergencyTypeToString(type));
    Serial.print("Description: ");
    Serial.println(description);
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    
    currentEmergency.type = type;
    currentEmergency.description = description;
    currentEmergency.timestamp = millis();
    currentEmergency.active = true;
    
    emergencyActive = true;
    emergencyStartTime = millis();
    
    addToHistory(currentEmergency);
    
    // Call callback if set
    if (onEmergencyStart != nullptr) {
        onEmergencyStart(type);
    }
}

void EmergencyHandler::clearEmergency() {
    if (!emergencyActive) return;
    
    Serial.println("Emergency cleared");
    Serial.print("Duration: ");
    Serial.print((millis() - emergencyStartTime) / 1000);
    Serial.println(" seconds");
    
    currentEmergency.active = false;
    emergencyActive = false;
    
    // Call callback if set
    if (onEmergencyEnd != nullptr) {
        onEmergencyEnd(currentEmergency.type);
    }
    
    currentEmergency.type = EMERGENCY_NONE;
    currentEmergency.description = "";
}

String EmergencyHandler::getEmergencyDescription() {
    if (!emergencyActive) return "No emergency";
    
    return emergencyTypeToString(currentEmergency.type) + ": " + 
           currentEmergency.description;
}

void EmergencyHandler::addToHistory(EmergencyEvent event) {
    if (historyCount < 10) {
        history[historyCount++] = event;
    } else {
        // Shift history
        for (int i = 0; i < 9; i++) {
            history[i] = history[i + 1];
        }
        history[9] = event;
    }
}

String EmergencyHandler::emergencyTypeToString(EmergencyType type) {
    switch (type) {
        case EMERGENCY_BUTTON: return "Button";
        case EMERGENCY_COLLISION: return "Collision";
        case EMERGENCY_SYSTEM_FAULT: return "System Fault";
        case EMERGENCY_BATTERY_CRITICAL: return "Battery Critical";
        case EMERGENCY_SENSOR_FAILURE: return "Sensor Failure";
        case EMERGENCY_COMMUNICATION_LOSS: return "Comm Loss";
        default: return "None";
    }
}