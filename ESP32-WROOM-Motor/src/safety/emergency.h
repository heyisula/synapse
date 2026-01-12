#ifndef EMERGENCY_H
#define EMERGENCY_H

#include <Arduino.h>
#include "../motor/movement.h"

// Emergency source tracking
enum EmergencySource {
    NONE,
    BUTTON,
    UART
};

class EmergencyStop {
private:
    uint8_t buttonPin;
    MovementController* movementController;
    
    bool emergencyActive;
    bool lastButtonState;
    unsigned long lastDebounceTime;
    const unsigned long debounceDelay = 50;
    
    volatile bool interruptTriggered;
    EmergencySource lastSource;
    
    static EmergencyStop* instance;
    static void IRAM_ATTR handleInterrupt();

public:
    EmergencyStop(uint8_t pin, MovementController* movement);
    void begin();
    void update();
    
    bool isEmergencyActive();
    void activate(EmergencySource source);
    void toggle(EmergencySource source);
    void reset();
    
    EmergencySource getLastSource();
    bool isTriggeredByButton();
    bool isTriggeredByUART();
    
    bool checkButton();
};

#endif