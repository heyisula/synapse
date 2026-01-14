#include "hcsr04.h"
#include "../config/pins.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

UltrasonicManager::UltrasonicManager() {
    sensors[US_FRONT] = new NewPing(ULTRASONIC_FRONT_TRIG, ULTRASONIC_FRONT_ECHO, 200);
    sensors[US_BACK]  = new NewPing(ULTRASONIC_BACK_TRIG, ULTRASONIC_BACK_ECHO, 200);
    sensors[US_LEFT]  = new NewPing(ULTRASONIC_LEFT_TRIG, ULTRASONIC_LEFT_ECHO, 200);
    sensors[US_RIGHT] = new NewPing(ULTRASONIC_RIGHT_TRIG, ULTRASONIC_RIGHT_ECHO, 200);

    for(int i = 0; i < US_COUNT; i++) {
        filters[i] = new SimpleKalmanFilter(2.0, 2.0, 0.01);
        distances[i] = MAX_ULTRASONIC_DISTANCE;
        minValid[i] = 5.0;     // cm
        maxValid[i] = 180.0;   // cm
    }
    
    lastReadTime = 0;
    isUltrasonicMonitoringActive = false;
    lastCenterDistance = 0;
    lastLeftDistance = 0;
    lastRearDistance = 0;
    lastRightDistance = 0;
    
    currentSensorIndex = 0;
    lastSensorReadTime = 0;
}
UltrasonicManager::~UltrasonicManager() {
    // Clean up dynamically allocated memory
    for(int i = 0; i < US_COUNT; i++) {
        delete sensors[i];
        delete filters[i];
    }
}

void UltrasonicManager::begin() {
    Serial.println("Ultrasonic sensors initialized with Kalman filtering");
}

void UltrasonicManager::update() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastSensorReadTime >= 60) {
        // Read current sensor
        unsigned int rawDistance = sensors[currentSensorIndex]->ping_cm();
        
        if (rawDistance == 0 || rawDistance < minValid[currentSensorIndex]) {
            rawDistance = MAX_ULTRASONIC_DISTANCE;
        }
        
        float filteredDistance = filters[currentSensorIndex]->updateEstimate(rawDistance);
        
        if (filteredDistance < minValid[currentSensorIndex]) {
            distances[currentSensorIndex] = MAX_ULTRASONIC_DISTANCE;
        } else if (filteredDistance > maxValid[currentSensorIndex]) {
            distances[currentSensorIndex] = MAX_ULTRASONIC_DISTANCE;
        } else {
            distances[currentSensorIndex] = filteredDistance;
        }
        
        // Move to next sensor
        currentSensorIndex++;
        if (currentSensorIndex >= US_COUNT) {
            currentSensorIndex = 0;
        }
        
        lastSensorReadTime = currentTime;
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

        // Only print if there's a significant change (> 5cm)
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
}