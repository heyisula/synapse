#include "heartrate_sensor.h"
#include "../config/constants.h"
#include "../config/thresholds.h"
#include <Wire.h>

HeartRateSensor::HeartRateSensor()
    : heartRate(0),
      spO2(0),
      fingerDetected(false),
      lastReadTime(0),
      lastBeat(0),
      rateSpot(0),
      beatAvg(0),
      beatsPerMinute(0)
{
    memset(rates, 0, sizeof(rates));
}

bool HeartRateSensor::begin() {
    Wire.begin();
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("MAX30102 not found! Check wiring.");
        return false;
    }
    particleSensor.setup();
    particleSensor.setPulseAmplitudeRed(0x2F);
    particleSensor.setPulseAmplitudeGreen(0);
    Serial.println("MAX30102 initialized successfully");
    return true;
}

void HeartRateSensor::update() {
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime < 50) return;  // update interval
    lastReadTime = currentTime;

    long irValue = particleSensor.getIR();
    fingerDetected = (irValue > 35000);

    if (fingerDetected) {
        if (checkForBeat(irValue)) {
            long delta = currentTime - lastBeat;
            lastBeat = currentTime;

            beatsPerMinute = 60 / (delta / 1000.0);
            if (beatsPerMinute > 20 && beatsPerMinute < 255) {
                rates[rateSpot++] = (byte)beatsPerMinute;
                rateSpot %= RATE_SIZE;

                beatAvg = 0;
                for (byte i = 0; i < RATE_SIZE; i++) beatAvg += rates[i];
                beatAvg /= RATE_SIZE;

                heartRate = beatAvg;
            }
        }
        //SpO2 Calculation
        long redValue = particleSensor.getRed();
        if (redValue > 0 && irValue > 0) {
            float ratio = (float)redValue / (float)irValue;
            spO2 = (int)(110 - 25 * ratio);
            spO2 = constrain(spO2, SPO2_MIN, SPO2_MAX);
        }
    } else {
        heartRate = 0;
        spO2 = 0;
    }
}

int HeartRateSensor::getHeartRate() {
    return heartRate;
}

int HeartRateSensor::getSpO2() {
    return spO2;
}

bool HeartRateSensor::isFingerDetected() {
    return fingerDetected;
}

bool HeartRateSensor::isValid() {
    return heartRate >= HR_MIN && heartRate <= HR_MAX && spO2 >= SPO2_MIN && spO2 <= SPO2_MAX;
}

bool HeartRateSensor::isTachycardia() {
    return heartRate > HR_MAX;
}

bool HeartRateSensor::isBradycardia() {
    return heartRate > 0 && heartRate < HR_MIN;
}

bool HeartRateSensor::isLowOxygen() {
    return spO2 > 0 && spO2 < SPO2_MIN;
}