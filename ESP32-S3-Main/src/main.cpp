#include <Arduino.h>
#include "sensors/mpu6050.h"

MotionTracker motion;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("=== MotionTracker Test ===");

    if (!motion.begin()) {
        Serial.println("ERROR: MPU6050 not detected!");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("MPU6050 detected successfully.");
    delay(500);

    motion.autoCalibrate();
    Serial.println("Starting live data output...");
    Serial.println();
}

void loop() {
    motion.update();

    Serial.print("Pitch: ");
    Serial.print(motion.getPitch(), 2);
    Serial.print(" deg | Roll: ");
    Serial.print(motion.getRoll(), 2);
    Serial.print(" deg");

    Serial.print(" | Ax: ");
    Serial.print(motion.getAccelX(), 3);
    Serial.print(" g");

    Serial.print(" | Ay: ");
    Serial.print(motion.getAccelY(), 3);
    Serial.print(" g");

    Serial.print(" | Az: ");
    Serial.print(motion.getAccelZ(), 3);
    Serial.print(" g");

    Serial.print(" | FwdAcc: ");
    Serial.print(motion.getForwardAcceleration(), 3);
    Serial.print(" g");

    Serial.print(" | SideAcc: ");
    Serial.print(motion.getSideAcceleration(), 3);
    Serial.print(" g");

    Serial.println();

    delay(500); // ~20 Hz output
}
