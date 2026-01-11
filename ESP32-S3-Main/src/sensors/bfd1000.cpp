#include "bfd1000.h"
#include "../config/pins.h"
#include "../config/constants.h"

// Tunable constants (adjust if needed)
#define LINE_LOST_TIMEOUT     100   // ms before declaring line lost
#define DEFAULT_CALIBRATION_TIME 3000
#define SENSOR_COUNT          5

// High-resolution weights for high-speed PID
static const int weights[SENSOR_COUNT] = {
    -2000, -1000, 0, 1000, 2000
};

LineFollower::LineFollower() {
    sensorPins[0] = LINE_SENSOR_S1;
    sensorPins[1] = LINE_SENSOR_S2;
    sensorPins[2] = LINE_SENSOR_S3;
    sensorPins[3] = LINE_SENSOR_S4;
    sensorPins[4] = LINE_SENSOR_S5;

    for (int i = 0; i < SENSOR_COUNT; i++) {
        minValues[i] = 1023;
        maxValues[i] = 0;
        thresholds[i] = 512;
        sensorStates[i] = false;
    }

    linePosition = 0;
    lastLinePosition = 0;
    lastReadTime = 0;
    lastLineSeenTime = 0;
}

void LineFollower::begin() {
    for (int i = 0; i < SENSOR_COUNT; i++) {
        pinMode(sensorPins[i], INPUT);
    }
}

void LineFollower::calibrate(unsigned int duration) {
    unsigned long startTime = millis();

    while (millis() - startTime < duration) {
        for (int i = 0; i < SENSOR_COUNT; i++) {
            rawValues[i] = analogRead(sensorPins[i]);

            if (rawValues[i] < minValues[i])
                minValues[i] = rawValues[i];

            if (rawValues[i] > maxValues[i])
                maxValues[i] = rawValues[i];
        }
        delay(5);
    }

    for (int i = 0; i < SENSOR_COUNT; i++) {
        thresholds[i] = (minValues[i] + maxValues[i]) / 2;
    }
}

void LineFollower::update() {
    unsigned long currentTime = millis();

    if (currentTime - lastReadTime < SENSOR_READ_INTERVAL) return;
    lastReadTime = currentTime;

    int weightedSum = 0;
    int activeSensors = 0;

    for (int i = 0; i < SENSOR_COUNT; i++) {
         rawValues[i] = analogRead(sensorPins[i]);
        // Black line gives lower reflection → lower value
        sensorStates[i] = (rawValues[i] < thresholds[i]);

        if (sensorStates[i]) {
            weightedSum += weights[i];
            activeSensors++;
        }
    }

    if (activeSensors > 0) {
        lastLinePosition = linePosition;
        linePosition = weightedSum / activeSensors;
        lastLineSeenTime = currentTime;
    }
    else {
        // Line lost recovery logic
        // Push robot toward last known direction
        linePosition = (lastLinePosition >= 0) ? 2500 : -2500; //Right or Left
    }
}

bool LineFollower::isLineDetected() {
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (sensorStates[i]) return true;
    }
    return false;
}

bool LineFollower::isLineLost() {
    return (millis() - lastLineSeenTime) > LINE_LOST_TIMEOUT;
}

int LineFollower::getLinePosition() {
    return linePosition;
}

int LineFollower::getError() {
    // Error = deviation from center
    return linePosition;
}

bool LineFollower::isJunction() {
    int count = 0;
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (sensorStates[i]) count++;
    }
    return count >= 4;
}

bool LineFollower::isAllWhite() {
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (sensorStates[i]) return false;
    }
    return true;
}

bool LineFollower::isAllBlack() {
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (!sensorStates[i]) return false;
    }
    return true;
}
