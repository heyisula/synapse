#include <Arduino.h>

#include "modes/obstacle_avoidance.h"
#include "sensors/hcsr04.h"
#include "sensors/mpu6050.h"
#include "communication/uart.h"
#include "actuators/ermc1604syg.h"
#include "actuators/sfm27.h"

// --------------------------------------------------
// Global subsystem instances
// --------------------------------------------------
UltrasonicManager ultrasonicManager;
MotionTracker motionTracker;
UARTProtocol uartProtocol;
Display display;
Buzzer buzzer;

// --------------------------------------------------
// Obstacle Avoidance controller
// --------------------------------------------------
ObstacleAvoidance obstacleAvoidance(
    &ultrasonicManager,
    &motionTracker,
    &uartProtocol,
    &display,
    &buzzer
);

// --------------------------------------------------
// Setup
// --------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("======================================");
    Serial.println(" Obstacle Avoidance - SYSTEM TEST ");
    Serial.println("======================================");

    obstacleAvoidance.begin();

    Serial.println("Initialization complete.");
}

// --------------------------------------------------
// Loop
// --------------------------------------------------
void loop() {
    obstacleAvoidance.update();

    // ----------------------------------------------
    // Read back telemetry from ObstacleAvoidance
    // ----------------------------------------------
    ObstacleAvoidance::SensorData data = obstacleAvoidance.getSensorData();

    Serial.print("[STATUS=");
    Serial.print(data.status);
    Serial.print("] ");

    Serial.print("SPD=");
    Serial.print(data.speed);
    Serial.print("% | ");

    Serial.print("F:");
    Serial.print(data.frontDist, 1);
    Serial.print("cm ");

    Serial.print("B:");
    Serial.print(data.rearDist, 1);
    Serial.print("cm ");

    Serial.print("L:");
    Serial.print(data.leftDist, 1);
    Serial.print("cm ");

    Serial.print("R:");
    Serial.print(data.rightDist, 1);
    Serial.print("cm | ");

    Serial.print("Pitch:");
    Serial.print(data.pitch, 1);
    Serial.print(" ");

    Serial.print("Roll:");
    Serial.print(data.roll, 1);
    Serial.print(" | ");

    Serial.print("AccX:");
    Serial.print(data.forwardAccel, 2);
    Serial.print(" ");

    Serial.print("AccY:");
    Serial.print(data.sideAccel, 2);

    Serial.println();

    delay(250);
}
