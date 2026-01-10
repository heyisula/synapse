#include "emergency.h"

EmergencyStop* EmergencyStop::instance = nullptr;

EmergencyStop::EmergencyStop(uint8_t pin, MovementController* movement) 
    : interruptTriggered(false) {  // Initialize atomic variable in initializer list
    buttonPin = pin;
    movementController = movement;
    emergencyActive = false;
    lastButtonState = HIGH;
    lastDebounceTime = 0;
    instance = this;
}

void EmergencyStop::begin() {
    // Null check for movementController
    if (movementController == nullptr) {
        Serial.println("ERROR: Movement controller is null in EmergencyStop!");
        return;
    }
    
    pinMode(buttonPin, INPUT_PULLUP);
    
    // Attach interrupt for emergency button (active LOW)
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleInterrupt, FALLING);
    
    emergencyActive = false;
    Serial.println("Emergency Stop System Initialized");
}

void IRAM_ATTR EmergencyStop::handleInterrupt() {
    if (instance != nullptr) {
        // Use atomic store operation - thread-safe
        instance->interruptTriggered.store(true, std::memory_order_release);
    }
}

void EmergencyStop::update() {
    // Null check for movementController
    if (movementController == nullptr) {
        return;
    }
    
    // Check if interrupt was triggered using atomic exchange
    // This atomically reads and clears the flag in one operation
    if (interruptTriggered.exchange(false, std::memory_order_acquire)) {
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
    if (movementController == nullptr) {
        Serial.println("ERROR: Cannot activate emergency stop - movement controller is null!");
        return;
    }
    
    if (!emergencyActive) {
        emergencyActive = true;
        movementController->emergencyStop();
        Serial.println("⚠️ EMERGENCY STOP ACTIVATED!");
    }
}

void EmergencyStop::reset() {
    emergencyActive = false;
    Serial.println("Emergency Stop Reset");
}

bool EmergencyStop::isEmergencyActive() {
    return emergencyActive;
}