#include "led_array.h"
#include "../config/pins.h"

LEDArray::LEDArray() {
    leftPin = LED_ARRAY_LEFT;
    rightPin = LED_ARRAY_RIGHT;
    leftPattern = LED_OFF;
    rightPattern = LED_OFF;
    lastUpdateTime = 0;
    ledState = false;
}

void LEDArray::begin() {
    pinMode(leftPin, OUTPUT);
    pinMode(rightPin, OUTPUT);
    digitalWrite(leftPin, LOW);
    digitalWrite(rightPin, LOW);
}

void LEDArray::update() {
    unsigned long currentTime = millis();
// Update based on patterns
    if(leftPattern == LED_BLINK_SLOW && currentTime - lastUpdateTime >= 500) {
        ledState = !ledState;
        digitalWrite(leftPin, ledState);
        lastUpdateTime = currentTime;
    }
    
    if(rightPattern == LED_BLINK_SLOW && currentTime - lastUpdateTime >= 500) {
        ledState = !ledState;
        digitalWrite(rightPin, ledState);
        lastUpdateTime = currentTime;
    }
}

void LEDArray::setPattern(bool isLeft, LEDPattern pattern) {
    if(isLeft) {
        leftPattern = pattern;

if(pattern == LED_SOLID) digitalWrite(leftPin, HIGH);
        else if(pattern == LED_OFF) digitalWrite(leftPin, LOW);
    } else {
        rightPattern = pattern;
        if(pattern == LED_SOLID) digitalWrite(rightPin, HIGH);
        else if(pattern == LED_OFF) digitalWrite(rightPin, LOW);
    }
}

void LEDArray::setBoth(LEDPattern pattern) {
    setPattern(true, pattern);
    setPattern(false, pattern);
}

void LEDArray::setStatus(bool left, bool right) {
    digitalWrite(leftPin, left ? HIGH : LOW);
    digitalWrite(rightPin, right ? HIGH : LOW);
}