#include "ky040.h"
#include "../config/pins.h"

// State table could be used, but bit manipulation is also efficient.
RotaryEncoder::RotaryEncoder() {
    pinCLK = ROTARY_CLK;
    pinDT = ROTARY_DT;
    pinSW = ROTARY_SW;
    
    lastEncoded = 0;
    encoderDelta = 0;
    
    buttonState = HIGH; // Input pullup defaults high
    lastButtonState = HIGH;
    buttonPressed = false;
    buttonReleased = false;
    longPressActive = false;
    
    lastDebounceTime = 0;
    buttonPressTime = 0;
}

RotaryEncoder::~RotaryEncoder() {
    // modifying pins isn't really needed in destructor
}

void RotaryEncoder::begin() {
    pinMode(pinCLK, INPUT_PULLUP);
    pinMode(pinDT, INPUT_PULLUP);
    pinMode(pinSW, INPUT_PULLUP);
    
    // Read initial state
    int MSB = digitalRead(pinDT);
    int LSB = digitalRead(pinCLK);
    
    // Combine to get 2-bit state (0-3)
    lastEncoded = (MSB << 1) | LSB;
}

void RotaryEncoder::update() {
    // --- 1. Encoder Logic (State Machine) ---
    int MSB = digitalRead(pinDT);
    int LSB = digitalRead(pinCLK);
    int encoded = (MSB << 1) | LSB;
    
    // Current state (encoded) and previous state (lastEncoded) form a 4-bit number
    // sum = 0b[Old_MSB][Old_LSB][New_MSB][New_LSB]
    int sum = (lastEncoded << 2) | encoded;
    
    // Verify validity using the "sum" transition
    // Valid CW transitions: 0b0010 (2), 0b1011 (11), 0b1101 (13), 0b0100 (4)
    // Valid CCW transitions: 0b0001 (1), 0b0111 (7), 0b1110 (14), 0b1000 (8)
    
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        encoderDelta++;
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        encoderDelta--;
    }
    
    lastEncoded = encoded;

    // --- 2. Button Logic (Non-blocking Debounce) ---
    int reading = digitalRead(pinSW);
    unsigned long currentTime = millis();

    // Reset debounce timer if reading changed (noise or actual press)
    if (reading != lastButtonState) {
        lastDebounceTime = currentTime;
    }
    
    // Check if the state has been stable for the debounce delay
    if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
        
        // If the state has changed
        if (reading != buttonState) {
            buttonState = reading;
            
            // Logic: Button is Active LOW
            if (buttonState == LOW) {
                // Just Pressed
                buttonPressTime = currentTime;
                longPressActive = false; // Reset long press flag
                // buttonPressed = true; // If we wanted press-trigger
            } else {
                // Just Released
                // Only trigger release if it wasn't a long press
                if (!longPressActive) {
                    buttonReleased = true;
                }
            }
        }
        
        // Update last stable state only after debounce confirmation? 
        // Actually, we usually update lastButtonState immediately on change to sniff noise.
        // But here we want the 'logic' state.
        
        // Handle Long Press while held down
        if (buttonState == LOW && !longPressActive) {
            if ((currentTime - buttonPressTime) > LONG_PRESS_DELAY) {
                longPressActive = true;
                // Force exit or action immediately on long press detection?
                // Or leave it to the getter to check
            }
        }
    }
    
    lastButtonState = reading;
}

int RotaryEncoder::getDelta() {
    int d = encoderDelta;
    encoderDelta = 0; // Reset after reading to consume the event
    return d;
}

bool RotaryEncoder::isButtonPressed() {
    // Not using this for the menu, relying on release, but keeping for API drift
    if (buttonPressed) {
        buttonPressed = false;
        return true;
    }
    return false;
}

bool RotaryEncoder::isButtonReleased() {
    if (buttonReleased) {
        buttonReleased = false;
        return true;
    }
    return false;
}

bool RotaryEncoder::isLongPress() {
    // We return true continuously or once? 
    // Usually once is safer for menus.
    if (longPressActive) {
        // To prevent spamming long press, we can reset it, 
        // OR we can rely on the caller to handle repeating.
        // Let's make it return true ONCE per press-hold.
        longPressActive = false; // Consume the event
        // Also suppress the release event that will come later
        return true;
    }
    return false;
}