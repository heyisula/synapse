#include "ultrasonic.h"
#include "../config/pins.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

Ultrasonic::Ultrasonic() {
    trigPins[US_FRONT] = ULTRASONIC_FRONT_TRIG;
    trigPins[US_BACK] = ULTRASONIC_BACK_TRIG;
    trigPins[US_LEFT] = ULTRASONIC_LEFT_TRIG;
    trigPins[US_RIGHT] = ULTRASONIC_RIGHT_TRIG;
    
    echoPins[US_FRONT] = ULTRASONIC_FRONT_ECHO;
    echoPins[US_BACK] = ULTRASONIC_BACK_ECHO;
    echoPins[US_LEFT] = ULTRASONIC_LEFT_ECHO;
    echoPins[US_RIGHT] = ULTRASONIC_RIGHT_ECHO;
    
    for(int i = 0; i < US_COUNT; i++) {
        distances[i] = MAX_ULTRASONIC_DISTANCE;
    }
    lastReadTime = 0;
}

void Ultrasonic::begin() {
    for(int i = 0; i < US_COUNT; i++) {
        pinMode(trigPins[i], OUTPUT);
        pinMode(echoPins[i], INPUT);
        digitalWrite(trigPins[i], LOW);
    }
}

float Ultrasonic::measureDistance(UltrasonicPosition pos) {
    digitalWrite(trigPins[pos], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[pos], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[pos], LOW);
    
    long duration = pulseIn(echoPins[pos], HIGH, 30000);
    
    if(duration == 0) {
        return MAX_ULTRASONIC_DISTANCE;
    }
    
    float distance = duration * SOUND_SPEED / 2.0;
    
    if(distance > MAX_ULTRASONIC_DISTANCE) {
        distance = MAX_ULTRASONIC_DISTANCE;
    }
    
    return distance;
}

void Ultrasonic::update() {
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= SENSOR_READ_INTERVAL) {
        for(int i = 0; i < US_COUNT; i++) {
            distances[i] = measureDistance((UltrasonicPosition)i);
            delay(10); // Small delay between readings
        }
        lastReadTime = currentTime;
    }
}

float Ultrasonic::getDistance(UltrasonicPosition pos) {
    return distances[pos];
}

bool Ultrasonic::isObstacleDetected(UltrasonicPosition pos, float threshold) {
    return distances[pos] < threshold;
}

bool Ultrasonic::isEmergencyStop() {
    return distances[US_FRONT] < EMERGENCY_STOP_DISTANCE;
}