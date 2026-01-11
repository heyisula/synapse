#ifndef MOTION_TRACKER_H
#define MOTION_TRACKER_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

class MotionTracker {
private:
    MPU6050 mpu;

    // Raw accelerometer (g)
    float accelX;
    float accelY;
    float accelZ;

    // Raw gyroscope (deg/s)
    float gyroX;
    float gyroY;

    // Filtered tilt angles (degrees)
    float pitch;   // Left / Right tilt (Y-axis)
    float roll;    // Front / Back tilt (X-axis)

    // Linear accelerations (gravity removed)
    float forwardAccel;  // X-axis
    float sideAccel;     // Y-axis

    unsigned long lastUpdateTime;

    // Filter constants (vehicle optimized)
    static constexpr float ALPHA = 0.93f;
    static constexpr float GYRO_DEADBAND  = 0.3f;
    static constexpr float ACCEL_DEADBAND = 0.015f;

public:
    MotionTracker(TwoWire &w = Wire);

    bool begin();
    void calibrate();
    void update();

    // Tilt angles
    float getPitch();   // Left / Right tilt
    float getRoll();    // Front / Back tilt

    // Linear acceleration
    float getForwardAcceleration();
    float getSideAcceleration();

    // Raw accel (optional diagnostics)
    float getAccelX();
    float getAccelY();
    float getAccelZ();

    // Acceleration magnitude (impact detection)
    float getAccelMagnitude();
};

#endif
