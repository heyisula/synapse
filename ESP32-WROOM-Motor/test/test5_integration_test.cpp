#include <Arduino.h>
#include "communication/uart.h"
#include "motor/l298n.h"
#include "motor/movement.h"
#include "motor/speed.h"
#include "safety/emergency.h"
#include "config/pins.h"

// Enable debug for testing
#define DEBUG_MODE
#include "config/debug.h"

UARTProtocol uart;
L298NController motorController;
MovementController movementController(&motorController);
SpeedController speedController;
EmergencyStop emergencyStop(EMERGENCY_STOP_PIN, &movementController);

void testCommand(MotorCommand cmd, uint8_t speed, const char* name) {
    Serial.printf("\n--- Testing: %s (Speed: %d) ---\n", name, speed);
    
    movementController.executeCommand(cmd, speed);
    delay(2000);  // Run for 2 seconds
    
    movementController.stop();
    Serial.println("✓ Command completed\n");
    delay(1000);  // Pause between commands
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n========================================");
    Serial.println("   Complete System Integration Test");
    Serial.println("========================================\n");
    
    Serial.println("Initializing systems...");
    uart.begin();
    motorController.begin();
    movementController.begin();
    speedController.begin();
    emergencyStop.begin();
    
    Serial.println("✓ All systems initialized\n");
    Serial.println("⚠️ WARNING: Motors will start in 5 seconds!");
    Serial.println("Make sure robot is elevated or in safe area\n");
    
    delay(5000);
}

void loop() {
    emergencyStop.update();
    
    if (emergencyStop.isEmergencyActive()) {
        Serial.println("⚠️ Emergency stop is active!");
        Serial.println("Reset the system to continue testing\n");
        delay(1000);
        return;
    }
    
    Serial.println("\n========================================");
    Serial.println("Starting Movement Test Sequence");
    Serial.println("========================================");
    
    // Test all movement commands
    testCommand(CMD_FORWARD, 50, "FORWARD at 50%");
    testCommand(CMD_FORWARD, 100, "FORWARD at 100%");
    
    testCommand(CMD_BACKWARD, 50, "BACKWARD at 50%");
    testCommand(CMD_BACKWARD, 100, "BACKWARD at 100%");
    
    testCommand(CMD_LEFT, 60, "TURN LEFT at 60%");
    testCommand(CMD_RIGHT, 60, "TURN RIGHT at 60%");
    
    testCommand(CMD_ROTATE_LEFT, 50, "ROTATE LEFT at 50%");
    testCommand(CMD_ROTATE_RIGHT, 50, "ROTATE RIGHT at 50%");
    
    // Test stop
    Serial.println("\n--- Testing: STOP ---");
    movementController.stop();
    delay(2000);
    Serial.println("✓ Stop completed\n");
    
    Serial.println("\n========================================");
    Serial.println("✓ All movement tests completed!");
    Serial.println("Waiting 10 seconds before repeating...");
    Serial.println("========================================\n");
    
    delay(10000);
}