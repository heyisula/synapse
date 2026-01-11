#include "le0066.h"
#include "../config/pins.h"

LEDArray::LEDArray() {
    leftPin = LED_LEFT;
    rightPin = LED_RIGHT;
    leftBrightness = 0;
    rightBrightness = 0;
}

void LEDArray::begin() {
    pinMode(leftPin, OUTPUT);
    pinMode(rightPin, OUTPUT);

    analogWrite(leftPin, 0);
    analogWrite(rightPin, 0);
}

void LEDArray::setBrightness(uint8_t left, uint8_t right) {
    leftBrightness = left;
    rightBrightness = right;
    
    // Apply immediately if the LEDs are considered "on"
    analogWrite(leftPin, leftBrightness);
    analogWrite(rightPin, rightBrightness);
}

void LEDArray::setBothBrightness(uint8_t brightness) {
    setBrightness(brightness, brightness);
}

void LEDArray::setStatus(bool leftOn, bool rightOn) {
    analogWrite(leftPin, leftOn ? leftBrightness : 0);
    analogWrite(rightPin, rightOn ? rightBrightness : 0);
}
