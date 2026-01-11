#include "sensors/mpu6050.h"
#include <math.h>
#include "config/pins.h"
#include "config/constants.h"

MotionTracker::MotionTracker(TwoWire &w) : mpu(w) {
    accelX = accelY = accelZ = 0.0f;
    gyroX  = gyroY  = 0.0f;

    pitch = roll = 0.0f;
    forwardAccel = sideAccel = 0.0f;

    lastUpdateTime = 0;
}

bool MotionTracker::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);

    mpu.begin();
    delay(1000);

    lastUpdateTime = millis();
    return true;
}

void MotionTracker::calibrate() {
    delay(2000);              // Keep robot still
    mpu.calcGyroOffsets(true);

    pitch = 0.0f;
    roll  = 0.0f;
}

void MotionTracker::update() {
    unsigned long now = millis();
    float dt = (now - lastUpdateTime) * 0.001f;
    if (dt <= 0.0f) return;
    lastUpdateTime = now;

    mpu.update();

    // --- Read sensor data (rotated 90°) ---
    float ax_sensor = mpu.getAccX();
    float ay_sensor = mpu.getAccY();
    float az_sensor = mpu.getAccZ();
    
    float gx_sensor = mpu.getGyroX();
    float gy_sensor = mpu.getGyroY();

    // --- REMAP to robot frame (CORRECTED FOR YOUR SENSOR ORIENTATION) ---
    float ax_robot = -ay_sensor;       // Forward/Backward
    float ay_robot = ax_sensor;        // Side-to-Side
    float az_robot = az_sensor;        // Up/Down
    
    float gyroX_robot = -gy_sensor;    // Roll rate
    float gyroY_robot = gx_sensor;     // Pitch rate

    // --- Apply deadbands ---
    accelX = (fabs(ax_robot) < ACCEL_DEADBAND) ? 0.0f : ax_robot;
    accelY = (fabs(ay_robot) < ACCEL_DEADBAND) ? 0.0f : ay_robot;
    accelZ = az_robot;

    gyroX = (fabs(gyroX_robot) < GYRO_DEADBAND) ? 0.0f : gyroX_robot;
    gyroY = (fabs(gyroY_robot) < GYRO_DEADBAND) ? 0.0f : gyroY_robot;

    // --- Rest of the code remains the same ---
    float pitchAcc = atan2(accelY, accelZ) * RAD_TO_DEG;
    float rollAcc = atan2(-accelX, accelZ) * RAD_TO_DEG;

    pitch += gyroY * dt;
    roll += gyroX * dt;

    pitch = ALPHA * pitch + (1.0f - ALPHA) * pitchAcc;
    roll = ALPHA * roll + (1.0f - ALPHA) * rollAcc;

    forwardAccel = accelX - sin(roll * DEG_TO_RAD);
    sideAccel = accelY - sin(pitch * DEG_TO_RAD);
}

// ---------------- Getters ----------------

float MotionTracker::getPitch() {
    return pitch;
}

float MotionTracker::getRoll() {
    return roll;
}

float MotionTracker::getForwardAcceleration() {
    return forwardAccel;
}

float MotionTracker::getSideAcceleration() {
    return sideAccel;
}

float MotionTracker::getAccelX() {
    return accelX;
}

float MotionTracker::getAccelY() {
    return accelY;
}

float MotionTracker::getAccelZ() {
    return accelZ;
}

float MotionTracker::getAccelMagnitude() {
    return sqrt(accelX * accelX +
                accelY * accelY +
                accelZ * accelZ);
}
