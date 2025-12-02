#ifndef MOTION_TRACKER_H
#define MOTION_TRACKER_H

#include <Arduino.h>
#include <Wire.h>

class MotionTracker {
private:
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
    float pitch, roll, yaw;
    int16_t rawAccelX, rawAccelY, rawAccelZ;
    int16_t rawGyroX, rawGyroY, rawGyroZ;
    unsigned long lastReadTime;
    
    const uint8_t MPU6050_ADDR = 0x68;
    const uint8_t PWR_MGMT_1 = 0x6B;
    const uint8_t ACCEL_XOUT_H = 0x3B;
    
public:
    MotionTracker();
    bool begin();
    void update();
    void calibrate();
    float getPitch();
    float getRoll();
    float getYaw();
    bool isTilted(float threshold);
};

#endif