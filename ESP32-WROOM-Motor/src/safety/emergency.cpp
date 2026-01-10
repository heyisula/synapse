#include "emergency.h"
#include "../config/debug.h"

EmergencyStop* EmergencyStop::instance = nullptr;

EmergencyStop::EmergencyStop(uint8_t pin, MovementController* movement) {
    buttonPin = pin;
    movementController = movement;
    emergencyActive = false;
    lastButtonState = HIGH;
    lastDebounceTime = 0;
    interruptTriggered = false;
    instance = this;
}

void EmergencyStop::begin() {
    pinMode(buttonPin, INPUT_PULLUP);
    
    // Attach interrupt for emergency button (active LOW)
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleInterrupt, FALLING);
    
    emergencyActive = false;
    DEBUG_PRINTLN("Emergency Stop System Initialized");
}

void IRAM_ATTR EmergencyStop::handleInterrupt() {
    if (instance != nullptr) {
        instance->interruptTriggered = true;
    }
}

void EmergencyStop::update() {
    // Check if interrupt was triggered
    if (interruptTriggered) {
        interruptTriggered = false;
        activate();
    }
    
    // Also poll the button state for redundancy
    checkButton();
}

bool EmergencyStop::checkButton() {
    bool currentState = digitalRead(buttonPin);
    
    // Button pressed (active LOW)
    if (currentState == LOW && lastButtonState == HIGH) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = millis();
            activate();
            lastButtonState = currentState;
            return true;
        }
    }
    
    lastButtonState = currentState;
    return false;
}

void EmergencyStop::activate() {
    if (!emergencyActive) {
        emergencyActive = true;
        movementController->emergencyStop();
        DEBUG_PRINTLN("⚠️ EMERGENCY STOP ACTIVATED!");
    }
}

void EmergencyStop::reset() {
    emergencyActive = false;
    DEBUG_PRINTLN("Emergency Stop Reset");
}

bool EmergencyStop::isEmergencyActive() {
    return emergencyActive;
}