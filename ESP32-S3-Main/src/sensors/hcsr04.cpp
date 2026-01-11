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

