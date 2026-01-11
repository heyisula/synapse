#include <Arduino.h>
#include "sensors/mpu6050.h"

MotionTracker motionTracker;
unsigned long lastPrintTime = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);
    
    Serial.println("\n=== MPU6050 Motion Tracker ===");
    Serial.println("Initializing...\n");
    
    if (!motionTracker.begin()) {
        Serial.println("ERROR: MPU6050 initialization failed!");
        while (1) delay(1000);
    }
    
    Serial.println("MPU6050 initialized successfully!");
    
    motionTracker.autoCalibrate();
    
    Serial.println("--- Starting measurements ---\n");
    Serial.println("Time(s) | Roll(°) | Pitch(°) | Fwd(g) | Side(g) | Mag(g) | RawX | RawY | RawZ");
    Serial.println("--------|---------|----------|--------|---------|--------|------|------|------");
    
    lastPrintTime = millis();
}

void loop() {
    motionTracker.update();
    
    if (millis() - lastPrintTime >= 100) {
        lastPrintTime = millis();
        
        float roll = motionTracker.getRoll();
        float pitch = motionTracker.getPitch();
        float fwd = motionTracker.getForwardAcceleration();
        float side = motionTracker.getSideAcceleration();
        float mag = motionTracker.getAccelMagnitude();
        float rawX = motionTracker.getAccelX();
        float rawY = motionTracker.getAccelY();
        float rawZ = motionTracker.getAccelZ();
        
        char buf[150];
        snprintf(buf, sizeof(buf),
                 "%7.2f | %7.2f | %8.2f | %6.3f | %7.3f | %6.3f | %4.2f | %4.2f | %4.2f",
                 millis()/1000.0, roll, pitch, fwd, side, mag, rawX, rawY, rawZ);
        Serial.println(buf);
    }
}
