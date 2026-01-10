#ifndef EMERGENCY_H
#define EMERGENCY_H

#include <Arduino.h>
#include <atomic>
#include "motor/movement.h"

class EmergencyStop {
private:
    uint8_t buttonPin;
    MovementController* movementController;
    
    bool emergencyActive;
    bool lastButtonState;
    unsigned long lastDebounceTime;
    const unsigned long debounceDelay = 50;
    
    std::atomic<bool> interruptTriggered;
    
    static EmergencyStop* instance;
    static void IRAM_ATTR handleInterrupt();

public:
    EmergencyStop(uint8_t pin, MovementController* movement);
    void begin();
    void update();
    
    bool isEmergencyActive();
    void activate();
    void reset();
    
    bool checkButton();
};

#endif