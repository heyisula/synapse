#include <math.h>
#include "sensors/mpu6050.h"
#include "config/pins.h"
#include "config/constants.h"

MotionTracker::MotionTracker() : mpu(0x68) {
    accelX = accelY = accelZ = 0.0f;
    gyroX = gyroY = 0.0f;
    pitch = roll = 0.0f;
    forwardAccel = sideAccel = 0.0f;
    
    accelXOffset = accelYOffset = 0.0f;
    rollOffset = pitchOffset = 0.0f;
    gyroXOffset = gyroYOffset = 0.0f;
    
    lastUpdateTime = 0;
}

bool MotionTracker::begin() {
    mpu.initialize();
    delay(50);
    
    // Verify connection
    uint8_t whoami = mpu.getDeviceID();
    if (whoami != 0x68 && whoami != 0x34) { 
        Serial.print("MPU6050 ID check failed: 0x");
        Serial.println(whoami, HEX);
        return false;
    }
    
    // Configure sensor
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);   // ±2g
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);   // ±250°/s
    mpu.setDLPFMode(MPU6050_DLPF_BW_20);              // 20Hz filter
    
    lastUpdateTime = millis();
    return true;
}


void MotionTracker::autoCalibrate() {
    Serial.println("Starting auto-calibration...");
    Serial.println("Keep robot STILL and LEVEL for 3 seconds!");
    
    delay(1000);
    
    const int samples = 100;
    float sumAX = 0, sumAY = 0, sumGX = 0, sumGY = 0;
    float sumRoll = 0, sumPitch = 0;
    
    Serial.println("Sampling accelerometer and tilt angles...");
    
    for (int i = 0; i < samples; i++) {
        int16_t ax, ay, az, gx, gy, gz;
        mpu.getAcceleration(&ax, &ay, &az);
        mpu.getRotation(&gx, &gy, &gz);
        
        // Convert to physical units
        float accel_x = ax / 16384.0f;
        float accel_y = ay / 16384.0f;
        float accel_z = az / 16384.0f;
        float gyro_x_raw = gx / 131.0f;
        float gyro_y_raw = gy / 131.0f;
        
        // Remap axes (sensor rotated 90° clockwise)
        // Sensor X → Robot forward/back
        // Sensor Y → Robot left/right (negated)
        float ax_robot = accel_x;
        float ay_robot = -accel_y;
        float az_robot = accel_z;
        
        // Calculate tilt angles
        float pitchAcc = atan2(ay_robot, az_robot) * RAD_TO_DEG;
        float rollAcc = atan2(-ax_robot, az_robot) * RAD_TO_DEG;
        
        sumAX += ax_robot;
        sumAY += ay_robot;
        sumGX += gyro_x_raw;
        sumGY += gyro_y_raw;
        sumRoll += rollAcc;
        sumPitch += pitchAcc;
        
        delay(10);
    }
    
    // Calculate offsets
    accelXOffset = sumAX / samples;
    accelYOffset = sumAY / samples;
    gyroXOffset = sumGX / samples;
    gyroYOffset = sumGY / samples;
    rollOffset = sumRoll / samples;
    pitchOffset = sumPitch / samples;
    
    pitch = 0.0f;
    roll = 0.0f;
    
    Serial.println("Calibration complete!");
    Serial.print("Accel X Offset: "); Serial.println(accelXOffset, 4);
    Serial.print("Accel Y Offset: "); Serial.println(accelYOffset, 4);
    Serial.print("Roll Offset: "); Serial.print(rollOffset, 2); Serial.println("°");
    Serial.print("Pitch Offset: "); Serial.print(pitchOffset, 2); Serial.println("°");
    Serial.println();
}

void MotionTracker::update() {
    unsigned long now = millis();
    float dt = (now - lastUpdateTime) * 0.001f;
    if (dt <= 0.0f) return;
    lastUpdateTime = now;

    // Read raw sensor data
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);
    
    // Convert to physical units
    float accel_x = ax / 16384.0f;
    float accel_y = ay / 16384.0f;
    float accel_z = az / 16384.0f;
    float gyro_x_raw = gx / 131.0f;
    float gyro_y_raw = gy / 131.0f;

    // Remap axes for your sensor orientation
    // User Spec: +X forward, +Y left
    // We use a body frame where X=Forward, Y=Left, Z=Up
    float ax_robot = accel_x; 
    float ay_robot = accel_y; // +Y is Left
    float az_robot = accel_z;
    
    // Gyro Axes: 
    // Rotation around X (Forward) is Roll Rate.
    // Rotation around Y (Left) is Pitch Rate.
    // Rotation around Z (Up) is Yaw Rate.
    float gyroX_robot = gyro_x_raw - gyroXOffset;
    float gyroY_robot = gyro_y_raw - gyroYOffset; 

    // Apply deadbands
    accelX = (fabs(ax_robot) < ACCEL_DEADBAND) ? 0.0f : ax_robot;
    accelY = (fabs(ay_robot) < ACCEL_DEADBAND) ? 0.0f : ay_robot;
    accelZ = az_robot;
    
    gyroX = (fabs(gyroX_robot) < GYRO_DEADBAND) ? 0.0f : gyroX_robot;
    gyroY = (fabs(gyroY_robot) < GYRO_DEADBAND) ? 0.0f : gyroY_robot;
    
    // Calculate tilt from accelerometer
    // Pitch (Nose Up/Down): Rotation around Y axis. Depends on X-axis Gravity.
    // +X is Forward. Nose Up -> +X sees +g component? (Depends on sensor). 
    // Usually Pitch = atan2(accelX, accelZ).
    float pitchAcc = atan2(ax_robot, az_robot) * RAD_TO_DEG - pitchOffset;       

    // Roll (Left/Right Tilt): Rotation around X axis. Depends on Y-axis Gravity.
    // +Y is Left. Left Wing Down -> +Y sees +g component.
    float rollAcc = atan2(ay_robot, az_robot) * RAD_TO_DEG - rollOffset;

    // Complementary Filter
    // Pitch rate is around Y axis.
    pitch += gyroY * dt;
    // Roll rate is around X axis.
    roll += gyroX * dt;

    pitch = MPU_ALPHA * pitch + (1.0f - MPU_ALPHA) * pitchAcc;
    roll = MPU_ALPHA * roll + (1.0f - MPU_ALPHA) * rollAcc;

    forwardAccel = ax_robot - sin(pitch * DEG_TO_RAD); // Remove gravity from Forward
    sideAccel = ay_robot - sin(roll * DEG_TO_RAD);     // Remove gravity from Side
}

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

int MotionTracker::getAccelerationInt() {
    update();
    
    float accelMagnitude = getAccelMagnitude();
    float accelMS2 = accelMagnitude * 9.81f;
    int acceleration = (int)round(accelMS2 * 100.0f);
    
    return acceleration;
}

int MotionTracker::getAngularVelocityInt() {
    update();
    
    float angularVelocity = gyroX;
    int angular = (int)round(angularVelocity);
    
    return angular;
}

void MotionTracker::getMotionData(int& acceleration, int& angular) {
    update();
    
    float accelMagnitude = getAccelMagnitude();
    float accelMS2 = accelMagnitude * 9.81f;
    
    // Return acceleration in cm/s² to preserve precision in int
    acceleration = (int)round(accelMS2 * 100.0f);
    
    // User requested "Robots Left and Right Angle" (Roll)
    // Positive Roll = Tilt Right (usually), depends on atan2 sign
    // Here we send the Angle in Degrees
    angular = (int)round(roll); 
    
    Serial.print("Motion - Accel: ");
    Serial.print(acceleration);
    Serial.print(" (");
    Serial.print(accelMS2, 2);
    Serial.print(" m/s²), Tilt Angle: ");
    Serial.print(angular);
    Serial.println("°");
}