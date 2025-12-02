#include "motion_tracker.h"
#include "../config/constants.h"

MotionTracker::MotionTracker() {
    accelX = accelY = accelZ = 0.0;
    gyroX = gyroY = gyroZ = 0.0;
    pitch = roll = yaw = 0.0;
    lastReadTime = 0;
}

bool MotionTracker::begin() {
    Wire.begin();
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(PWR_MGMT_1);
    Wire.write(0);
    uint8_t error = Wire.endTransmission();
    
    return error == 0;
}

void MotionTracker::update() {
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= SENSOR_READ_INTERVAL) {
        Wire.beginTransmission(MPU6050_ADDR);
        Wire.write(ACCEL_XOUT_H);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU6050_ADDR, (uint8_t)14);
        
        rawAccelX = Wire.read() << 8 | Wire.read();
        rawAccelY = Wire.read() << 8 | Wire.read();
        rawAccelZ = Wire.read() << 8 | Wire.read();
        Wire.read(); Wire.read(); // Temperature
        rawGyroX = Wire.read() << 8 | Wire.read();
        rawGyroY = Wire.read() << 8 | Wire.read();
        rawGyroZ = Wire.read() << 8 | Wire.read();
        
        // Convert to g's and degrees/sec
        accelX = rawAccelX / 16384.0;
        accelY = rawAccelY / 16384.0;
        accelZ = rawAccelZ / 16384.0;
        
        gyroX = rawGyroX / 131.0;
        gyroY = rawGyroY / 131.0;
        gyroZ = rawGyroZ / 131.0;
        
        // Calculate pitch and roll
        pitch = atan2(accelY, sqrt(accelX*accelX + accelZ*accelZ)) * 180.0 / PI;
        roll = atan2(-accelX, accelZ) * 180.0 / PI;
        
        lastReadTime = currentTime;
    }
}

void MotionTracker::calibrate() {
    Serial.println("Calibrating MPU6050...");
    delay(1000);
}

float MotionTracker::getPitch() {
    return pitch;
}

float MotionTracker::getRoll() {
    return roll;
}

float MotionTracker::getYaw() {
    return yaw;
}

bool MotionTracker::isTilted(float threshold) {
    return abs(pitch) > threshold || abs(roll) > threshold;
}