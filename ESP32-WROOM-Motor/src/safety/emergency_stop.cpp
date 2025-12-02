#include "emergency_stop.h"

EmergencyStop::EmergencyStop(uint8_t pin) 
    : buttonPin(pin), lastButtonState(HIGH), emergencyActive(false),
      lastDebounceTime(0), debounceDelay(50), 
      useInternalPullup(true), activeState(LOW) {}

void EmergencyStop::begin() {
    if (useInternalPullup) {
        pinMode(buttonPin, INPUT_PULLUP);
    } else {
        pinMode(buttonPin, INPUT);
    }
    
    lastButtonState = digitalRead(buttonPin);
    emergencyActive = false;
    
    Serial.println("Emergency Stop initialized");
    Serial.print("Button pin: ");
    Serial.println(buttonPin);
    Serial.print("Active state: ");
    Serial.println(activeState ? "HIGH" : "LOW");
}

void EmergencyStop::update() {
    bool currentState = readButton();
    
    // Check if button state changed
    if (currentState != lastButtonState) {
        lastDebounceTime = millis();
    }
    
    // Debounce delay passed
    if ((millis() - lastDebounceTime) > debounceDelay) {
        // If button is pressed (active state)
        if (currentState == activeState) {
            if (!emergencyActive) {
                emergencyActive = true;
                Serial.println("!!! EMERGENCY STOP ACTIVATED !!!");
            }
        }
    }
    
    lastButtonState = currentState;
}

bool EmergencyStop::isPressed() {
    return (readButton() == activeState);
}

bool EmergencyStop::readButton() {
    return digitalRead(buttonPin);
}

void EmergencyStop::reset() {
    if (!isPressed()) {
        emergencyActive = false;
        Serial.println("Emergency Stop reset");
    } else {
        Serial.println("Cannot reset - button still pressed!");
    }
}
