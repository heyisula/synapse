#include "hcsr04.h"
#include "../config/pins.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

UltrasonicManager::UltrasonicManager() {
    sensors[US_FRONT] = new Ultrasonic(ULTRASONIC_FRONT_TRIG, ULTRASONIC_FRONT_ECHO);
    sensors[US_BACK]  = new Ultrasonic(ULTRASONIC_BACK_TRIG, ULTRASONIC_BACK_ECHO);
    sensors[US_LEFT]  = new Ultrasonic(ULTRASONIC_LEFT_TRIG, ULTRASONIC_LEFT_ECHO);
    sensors[US_RIGHT] = new Ultrasonic(ULTRASONIC_RIGHT_TRIG, ULTRASONIC_RIGHT_ECHO);

    for(int i = 0; i < US_COUNT; i++) {
        distances[i] = MAX_ULTRASONIC_DISTANCE;
        minValid[i] = 5.0;     // cm
        maxValid[i] = 180.0;   // cm (real HC-SR04 limit)
    }
    lastReadTime = 0;
}

void UltrasonicManager::begin() {
}

void UltrasonicManager::update() {
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime >= SENSOR_READ_INTERVAL) {
        for (int i = 0; i < US_COUNT; i++) {
            distances[i] = sensors[i]->read() / 10.0;

            if (distances[i] < 2.0) {
                distances[i] = MAX_ULTRASONIC_DISTANCE; // ignore invalid
            }

            if (distances[i] > MAX_ULTRASONIC_DISTANCE) {
                distances[i] = MAX_ULTRASONIC_DISTANCE;
            }

            delayMicroseconds(60000); // 60 ms between sensors
        }
        lastReadTime = currentTime;
    }
}

float UltrasonicManager::getDistance(UltrasonicPosition pos) {
    return distances[pos];
}

bool UltrasonicManager::monitorUltrasonic(bool ultrasonic_start, 
                                         int& center, 
                                         int& left, 
                                         int& rear, 
                                         int& right) {
    if (ultrasonic_start) {
        if (!isUltrasonicMonitoringActive) {
            isUltrasonicMonitoringActive = true;
            Serial.println("=== Ultrasonic monitoring STARTED ===");
            lastCenterDistance = 0;
            lastLeftDistance = 0;
            lastRearDistance = 0;
            lastRightDistance = 0;
        }

        update();
        
        // Get current distances from all sensors
        float centerDist = getDistance(US_FRONT);
        float leftDist = getDistance(US_LEFT);
        float rearDist = getDistance(US_BACK);
        float rightDist = getDistance(US_RIGHT);

        center = (int)round(centerDist);
        left = (int)round(leftDist);
        rear = (int)round(rearDist);
        right = (int)round(rightDist);

        if (abs(center - lastCenterDistance) > 5 || 
            abs(left - lastLeftDistance) > 5 || 
            abs(rear - lastRearDistance) > 5 || 
            abs(right - lastRightDistance) > 5) {
            
            lastCenterDistance = center;
            lastLeftDistance = left;
            lastRearDistance = rear;
            lastRightDistance = right;
            
            Serial.println("┌─────────────────────────────────");
            Serial.print("│ Ultrasonic Distances (cm):");
            Serial.println();
            Serial.print("│   Center: ");
            Serial.print(center);
            Serial.println(" cm");
            Serial.print("│   Left:   ");
            Serial.print(left);
            Serial.println(" cm");
            Serial.print("│   Rear:   ");
            Serial.print(rear);
            Serial.println(" cm");
            Serial.print("│   Right:  ");
            Serial.print(right);
            Serial.println(" cm");
            Serial.println("└─────────────────────────────────");
        }
        
        return true;
        
    } else {
        if (isUltrasonicMonitoringActive) {
            isUltrasonicMonitoringActive = false;
            Serial.println("=== Ultrasonic monitoring STOPPED ===");
        }
        
        // Reset values
        center = 0;
        left = 0;
        rear = 0;
        right = 0;
        lastCenterDistance = 0;
        lastLeftDistance = 0;
        lastRearDistance = 0;
        lastRightDistance = 0;
        
        return false;
}