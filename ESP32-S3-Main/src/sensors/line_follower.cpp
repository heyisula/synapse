#include "line_follower.h"
#include "../config/pins.h"
#include "../config/constants.h"

LineFollower::LineFollower() {
    sensorPins[0] = LINE_SENSOR_S1;
    sensorPins[1] = LINE_SENSOR_S2;
    sensorPins[2] = LINE_SENSOR_S3;
    sensorPins[3] = LINE_SENSOR_S4;
    sensorPins[4] = LINE_SENSOR_S5;
    
    for(int i = 0; i < 5; i++) {
        sensorStates[i] = false;
    }
    linePosition = 0;
    lastReadTime = 0;
}

void LineFollower::begin() {
    for(int i = 0; i < 5; i++) {
        pinMode(sensorPins[i], INPUT);
    }
}

void LineFollower::update() {
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= SENSOR_READ_INTERVAL) {
        for(int i = 0; i < 5; i++) {
            sensorStates[i] = digitalRead(sensorPins[i]);
        }
        
        // Calculate line position (-2 to +2, where 0 is center)
        int weightedSum = 0;
        int activeSensors = 0;
        
        int weights[] = {-2, -1, 0, 1, 2};
        
        for(int i = 0; i < 5; i++) {
            if(sensorStates[i]) {
                weightedSum += weights[i];
                activeSensors++;
            }
        }
        
        if(activeSensors > 0) {
            linePosition = weightedSum / activeSensors;
        }
        
        lastReadTime = currentTime;
    }
}

bool LineFollower::isLineDetected() {
    for(int i = 0; i < 5; i++) {
        if(sensorStates[i]) return true;
    }
    return false;
}

int LineFollower::getLinePosition() {
    return linePosition;
}

int LineFollower::getError() {
    return linePosition;
}

bool LineFollower::isJunction() {
    int count = 0;
    for(int i = 0; i < 5; i++) {
        if(sensorStates[i]) count++;
    }
    return count >= 4;
}

bool LineFollower::isAllWhite() {
    for(int i = 0; i < 5; i++) {
        if(sensorStates[i]) return false;
    }
    return true;
}

bool LineFollower::isAllBlack() {
    for(int i = 0; i < 5; i++) {
        if(!sensorStates[i]) return false;
    }
    return true;
}