#include "ky040.h"
#include "../config/pins.h"

RotaryEncoder::RotaryEncoder() {
    swPin = ROTARY_SW;
    position = 0;
    buttonPressed = false;
    lastButtonTime = 0;
    buttonPressStart = 0;
    lastButtonState = false;
    lastSwState = HIGH;
    lastDebounceTime = 0;
    encoder = nullptr;
}

RotaryEncoder::~RotaryEncoder() {
    if (encoder) {
        delete encoder;
    }
}

void RotaryEncoder::begin() {
    // Initialize pins
    pinMode(ROTARY_CLK, INPUT_PULLUP);
    pinMode(ROTARY_DT, INPUT_PULLUP);
    pinMode(swPin, INPUT_PULLUP);
    
    encoder = new KY040(ROTARY_CLK, ROTARY_DT);
}

void RotaryEncoder::update() {
    if (!encoder) return;
    
    // Get rotation state and update position automatically
    byte rotation = encoder->getRotation();
    
    if (rotation == KY040::CLOCKWISE) {
        position++;
    } else if (rotation == KY040::COUNTERCLOCKWISE) {
        position--;
    }

    int reading = digitalRead(swPin);
    unsigned long currentTime = millis();
    
    // Check if the button state has changed
    if (reading != lastSwState) {
        lastDebounceTime = currentTime;
    }
    
    // Only register the change if it's been stable for the debounce delay
    if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
        // If the button state has actually changed
        if (reading != (buttonPressed ? LOW : HIGH)) {
            if (reading == LOW) {
                // Button pressed
                buttonPressed = true;
                buttonPressStart = currentTime;
            } else {
                // Button released
                buttonPressed = false;
            }
        }
    }
    
    lastSwState = reading;
}

int RotaryEncoder::getPosition() {
    return position;
}

void RotaryEncoder::resetPosition() {
    position = 0;
}

bool RotaryEncoder::isButtonPressed() {
    return buttonPressed;
}

bool RotaryEncoder::isButtonReleased() {
    bool released = lastButtonState && !buttonPressed;
    lastButtonState = buttonPressed;
    return released;
}

bool RotaryEncoder::isLongPress() {
    if (buttonPressed) {
        return (millis() - buttonPressStart) > 1000;
    }
    return false;
}