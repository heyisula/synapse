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

void LEDArray::update() {
    
}

void LEDArray::controlFromFirebase(int lightadj_left, int lightadj_right) {
    lightadj_left = constrain(lightadj_left, 0, 100);
    lightadj_right = constrain(lightadj_right, 0, 100);

    uint8_t leftPWM = 0;
    uint8_t rightPWM = 0;
    
    if (lightadj_left > 0) {
        leftPWM = map(lightadj_left, 1, 100, 1, 255);
    }
    
    if (lightadj_right > 0) {
        rightPWM = map(lightadj_right, 1, 100, 1, 255);
    }
    leftBrightness = leftPWM;
    rightBrightness = rightPWM;
    
    analogWrite(leftPin, leftPWM);
    analogWrite(rightPin, rightPWM);
    
    // Concise one-line log to avoid loop lag
    Serial.printf("LEDs: L:%d%% R:%d%%\n", lightadj_left, lightadj_right);
}