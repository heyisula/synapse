#include "rotary_encoder.h"
#include "../config/pins.h"

RotaryEncoder::RotaryEncoder() {
    clkPin = ROTARY_CLK;
    dtPin = ROTARY_DT;
    swPin = ROTARY_SW;
    position = 0;
    lastClkState = HIGH;
    buttonPressed = false;
    lastButtonTime = 0;
    buttonPressStart = 0;
}

void RotaryEncoder::begin() {
    pinMode(clkPin, INPUT_PULLUP);
    pinMode(dtPin, INPUT_PULLUP);
    pinMode(swPin, INPUT_PULLUP);
    lastClkState = digitalRead(clkPin);
}

void RotaryEncoder::update() {
    int clkState = digitalRead(clkPin);
    
    if(clkState != lastClkState && clkState == LOW) {
        if(digitalRead(dtPin) != clkState) {
            position++;
        } else {
            position--;
        }
    }
    lastClkState = clkState;
    
    // Button handling
    int swState = digitalRead(swPin);
    unsigned long currentTime = millis();
    
    if(swState == LOW && !buttonPressed) {
        if(currentTime - lastButtonTime > 50) { // Debounce
            buttonPressed = true;
            buttonPressStart = currentTime;
        }
    } else if(swState == HIGH && buttonPressed) {
        buttonPressed = false;
        lastButtonTime = currentTime;
    }
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
    static bool lastState = false;
    bool released = lastState && !buttonPressed;
    lastState = buttonPressed;
    return released;
}

bool RotaryEncoder::isLongPress() {
    if(buttonPressed) {
        return (millis() - buttonPressStart) > 1000;
    }
    return false;
}