#include "bfd1000.h"
#include "../config/pins.h"

BFD1000::BFD1000() {
    sensorPins[0] = LINE_SENSOR_S1;
    sensorPins[1] = LINE_SENSOR_S2;
    sensorPins[2] = LINE_SENSOR_S3;
    sensorPins[3] = LINE_SENSOR_S4;
    sensorPins[4] = LINE_SENSOR_S5;

    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        minValues[i] = 4095;  // ESP32 ADC max
        maxValues[i] = 0;
        thresholds[i] = 2048;
        rawValues[i] = 0;
        blackState[i] = false;
    }
}

void BFD1000::begin() {
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        pinMode(sensorPins[i], INPUT);
    }
}

void BFD1000::autoCalibrate(uint16_t durationMs) {
    unsigned long startTime = millis();

    while (millis() - startTime < durationMs) {
        for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
            int value = analogRead(sensorPins[i]);

            if (value < minValues[i]) minValues[i] = value;
            if (value > maxValues[i]) maxValues[i] = value;
        }
        delay(5);
    }

    // Set adaptive thresholds for black/white detection
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        thresholds[i] = (minValues[i] + maxValues[i]) / 2;
    }
}

void BFD1000::update() {
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        rawValues[i] = analogRead(sensorPins[i]);

        // BFD1000: black line → lower reflection → lower ADC
        blackState[i] = (rawValues[i] < thresholds[i]);
    }
}

int* BFD1000::getRawValues() {
    return rawValues;
}

bool BFD1000::isBlack(uint8_t index) {
    if (index >= SENSOR_COUNT) return false;
    return blackState[index];
}

bool BFD1000::isAnyBlack() {
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        if (blackState[i]) return true;
    }
    return false;
}

bool BFD1000::isAllBlack() {
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        if (!blackState[i]) return false;
    }
    return true;
}

bool BFD1000::isAllWhite() {
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        if (blackState[i]) return false;
    }
    return true;
}
