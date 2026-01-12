#include "emergency.h"
#include "../config/debug.h"

EmergencyStop* EmergencyStop::instance = nullptr;

EmergencyStop::EmergencyStop(uint8_t pin, MovementController* movement) : debounceDelay(300) { 
    buttonPin = pin;
    movementController = movement;
    emergencyActive = false;
    lastButtonState = HIGH;
    lastDebounceTime = 0;
    interruptTriggered = false;
    lastSource = NONE;
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
        
        // Debounce check
        if ((millis() - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = millis();
            toggle(BUTTON);
        }
    }
}

bool EmergencyStop::checkButton() {
    bool currentState = digitalRead(buttonPin);
    
    // Button pressed (active LOW) - detect HIGH to LOW edge
    if (currentState == LOW && lastButtonState == HIGH) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = millis();
            toggle(BUTTON);
            lastButtonState = currentState;
            return true;
        }
    }
    
    lastButtonState = currentState;
    return false;
}

void EmergencyStop::activate(EmergencySource source) {
    if (!emergencyActive) {
        emergencyActive = true;
        lastSource = source;
        movementController->emergencyStop();
        
        DEBUG_PRINT("⚠️ EMERGENCY STOP ACTIVATED! (Source: ");
        if (source == BUTTON) {
            DEBUG_PRINT("BUTTON");
        } else if (source == UART) {
            DEBUG_PRINT("UART");
        } else {
            DEBUG_PRINT("UNKNOWN");
        }
        DEBUG_PRINTLN(")");
    }
}

void EmergencyStop::toggle(EmergencySource source) {
    if (emergencyActive) {
        // Deactivate emergency
        emergencyActive = false;
        lastSource = NONE;
        DEBUG_PRINTLN("✓ Emergency Stop DEACTIVATED");
    } else {
        // Activate emergency
        activate(source);
    }
}

void EmergencyStop::reset() {
    emergencyActive = false;
    lastSource = NONE;
    DEBUG_PRINTLN("Emergency Stop Reset");
}

bool EmergencyStop::isEmergencyActive() {
    return emergencyActive;
}

EmergencySource EmergencyStop::getLastSource() {
    return lastSource;
}

bool EmergencyStop::isTriggeredByButton() {
    return emergencyActive && lastSource == BUTTON;
}

bool EmergencyStop::isTriggeredByUART() {
    return emergencyActive && lastSource == UART;
}