#include "led_array.h"
#include "../config/pins.h"

LEDArray::LEDArray() {
    leftPin = LED_LEFT;
    rightPin = LED_RIGHT;

    leftPattern = LED_OFF;
    rightPattern = LED_OFF;

    leftBrightness = 0;
    rightBrightness = 0;
    brightnessMode = false;

    lastUpdateTime = 0;
    ledState = false;
}

void LEDArray::begin() {
    pinMode(leftPin, OUTPUT);
    pinMode(rightPin, OUTPUT);

    digitalWrite(leftPin, LOW);
    digitalWrite(rightPin, LOW);
}
void LEDArray::setBrightness(uint8_t left, uint8_t right) {
    brightnessMode = true;
    leftBrightness = left;
    rightBrightness = right;

    analogWrite(leftPin, leftBrightness);
    analogWrite(rightPin, rightBrightness);
}

void LEDArray::setBothBrightness(uint8_t brightness) {
    setBrightness(brightness, brightness);
}

void LEDArray::setPattern(bool isLeft, LEDPattern pattern) {
    brightnessMode = false;  // Disable PWM mode

    if (isLeft) {
        leftPattern = pattern;
        if (pattern == LED_SOLID) digitalWrite(leftPin, HIGH);
        else if (pattern == LED_OFF) digitalWrite(leftPin, LOW);
    } else {
        rightPattern = pattern;
        if (pattern == LED_SOLID) digitalWrite(rightPin, HIGH);
        else if (pattern == LED_OFF) digitalWrite(rightPin, LOW);
    }
}

void LEDArray::setBoth(LEDPattern pattern) {
    setPattern(true, pattern);
    setPattern(false, pattern);
}

void LEDArray::setStatus(bool left, bool right) {
    brightnessMode = false;  // Disable PWM mode
    digitalWrite(leftPin, left ? HIGH : LOW);
    digitalWrite(rightPin, right ? HIGH : LOW);
}

void LEDArray::update() {
    if (brightnessMode) return;  // PWM mode, nothing to update

    unsigned long currentTime = millis();

    if (currentTime - lastUpdateTime >= 500) {
        if (leftPattern == LED_BLINK_SLOW) {
            digitalWrite(leftPin, ledState);
        }
        if (rightPattern == LED_BLINK_SLOW) {
            digitalWrite(rightPin, ledState);
        }

        ledState = !ledState;
        lastUpdateTime = currentTime;
    }
}