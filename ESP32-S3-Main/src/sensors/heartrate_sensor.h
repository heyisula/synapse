#ifndef HEARTRATE_SENSOR_H
#define HEARTRATE_SENSOR_H

#include <Arduino.h>
#include <MAX30105.h>
#include "heartRate.h"

class HeartRateSensor {
private:
    MAX30105 particleSensor;

    int heartRate;
    int spO2;
    bool fingerDetected;

    unsigned long lastReadTime;
    long  lastBeat;

    static constexpr byte RATE_SIZE = 4;
    byte rates[RATE_SIZE];
    byte rateSpot;
    int beatAvg;
    float beatsPerMinute;

    //static constexpr uint8_t MAX30102_ADDR = 0x57;
    
public:
    HeartRateSensor();
    bool begin();
    void update();
    int getHeartRate();
    int getSpO2();
    bool isFingerDetected();
    bool isValid();
    bool isTachycardia();
    bool isBradycardia();
    bool isLowOxygen();
};

#endif
