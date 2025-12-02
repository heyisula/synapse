#ifndef HEARTRATE_SENSOR_H
#define HEARTRATE_SENSOR_H

#include <Arduino.h>
class HeartRateSensor {
private:
    int heartRate;
    int spO2;
    bool fingerDetected;
    unsigned long lastReadTime;
    
    const uint8_t MAX30102_ADDR = 0x57;
    
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
