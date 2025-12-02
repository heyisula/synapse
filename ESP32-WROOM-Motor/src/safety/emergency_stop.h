#ifndef EMERGENCY_STOP_H
#define EMERGENCY_STOP_H

#include <Arduino.h>

class EmergencyStop {
private:
    uint8_t buttonPin;
    bool lastButtonState;
    bool emergencyActive;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;
    
    bool useInternalPullup;
    bool activeState;  // HIGH or LOW when pressed
    
public:
    EmergencyStop(uint8_t pin = 27);
    void begin();
    void update();
    
    bool isPressed();
    bool isActive() { return emergencyActive; }
    
    void setActiveState(bool state) { activeState = state; }
    void reset();
    
private:
    bool readButton();
};

#endif