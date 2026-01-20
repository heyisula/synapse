#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

class MotionTracker {
private:
    MPU6050 mpu;

    // Processed values
    float accelX, accelY, accelZ;
    float gyroX, gyroY;
    float pitch, roll;
    float forwardAccel, sideAccel;


    unsigned long lastUpdateTime;

    float accelXOffset;
    float accelYOffset;
    float rollOffset;
    float pitchOffset;
    float gyroXOffset;
    float gyroYOffset;

    static constexpr float ALPHA = 0.98f;  // 98% gyro, 2% accel
    static constexpr float GYRO_DEADBAND  = 0.3f;
    static constexpr float ACCEL_DEADBAND = 0.015f;

public:
    MotionTracker();

    bool begin();
    void autoCalibrate();
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

    int getAccelerationInt();
    int getAngularVelocityInt();
    void getMotionData(int& acceleration, int& angular);
};

#endif
