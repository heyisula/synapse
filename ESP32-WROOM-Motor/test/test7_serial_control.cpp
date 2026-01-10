#include <Arduino.h>
#include "communication/uart.h"
#include "motor/l298n.h"
#include "motor/movement.h"
#include "motor/speed.h"
#include "safety/emergency.h"
#include "config/pins.h"

#define DEBUG_MODE
#include "config/debug.h"

UARTProtocol uart;
L298NController motorController;
MovementController movementController(&motorController);
SpeedController speedController;
EmergencyStop emergencyStop(EMERGENCY_STOP_PIN, &movementController);

void printMenu() {
    Serial.println("\n========================================");
    Serial.println("        Manual Motor Control");
    Serial.println("========================================");
    Serial.println("Commands:");
    Serial.println("  w - Forward");
    Serial.println("  s - Backward");
    Serial.println("  a - Turn Left");
    Serial.println("  d - Turn Right");
    Serial.println("  q - Rotate Left");
    Serial.println("  e - Rotate Right");
    Serial.println("  x - STOP");
    Serial.println("  0-9 - Set speed (0=0%, 9=100%)");
    Serial.println("  h - Show this menu");
    Serial.println("  i - Show info");
    Serial.println("========================================\n");
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    uart.begin();
    motorController.begin();
    movementController.begin();
    speedController.begin();
    emergencyStop.begin();
    
    printMenu();
}

uint8_t currentSpeed = 50;  // Default 50%

void loop() {
    emergencyStop.update();
    
    if (Serial.available()) {
        char cmd = Serial.read();
        
        // Clear buffer
        while (Serial.available()) Serial.read();
        
        if (emergencyStop.isEmergencyActive() && cmd != 'r') {
            Serial.println("⚠️ Emergency stop active! Press 'r' to reset");
            return;
        }
        
        switch(cmd) {
            case 'w':
                Serial.printf("→ FORWARD at %d%%\n", currentSpeed);
                movementController.executeCommand(CMD_FORWARD, currentSpeed);
                break;
                
            case 's':
                Serial.printf("← BACKWARD at %d%%\n", currentSpeed);
                movementController.executeCommand(CMD_BACKWARD, currentSpeed);
                break;
                
            case 'a':
                Serial.printf("↰ TURN LEFT at %d%%\n", currentSpeed);
                movementController.executeCommand(CMD_LEFT, currentSpeed);
                break;
                
            case 'd':
                Serial.printf("↱ TURN RIGHT at %d%%\n", currentSpeed);
                movementController.executeCommand(CMD_RIGHT, currentSpeed);
                break;
                
            case 'q':
                Serial.printf("↺ ROTATE LEFT at %d%%\n", currentSpeed);
                movementController.executeCommand(CMD_ROTATE_LEFT, currentSpeed);
                break;
                
            case 'e':
                Serial.printf("↻ ROTATE RIGHT at %d%%\n", currentSpeed);
                movementController.executeCommand(CMD_ROTATE_RIGHT, currentSpeed);
                break;
                
            case 'x':
                Serial.println("■ STOP");
                movementController.stop();
                break;
                
            case '0'...'9':
                currentSpeed = (cmd - '0') * 11;  // 0-99%
                if (currentSpeed > 100) currentSpeed = 100;
                Serial.printf("Speed set to: %d%%\n", currentSpeed);
                break;
                
            case 'r':
                emergencyStop.reset();
                Serial.println("✓ Emergency stop reset");
                break;
                
            case 'h':
                printMenu();
                break;
                
            case 'i':
                Serial.printf("\nCurrent Speed: %d%%\n", currentSpeed);
                Serial.printf("Motor State: %s\n", 
                             movementController.getIsMoving() ? "Moving" : "Stopped");
                Serial.printf("Emergency: %s\n", 
                             emergencyStop.isEmergencyActive() ? "ACTIVE" : "Inactive");
                break;
                
            default:
                Serial.println("Unknown command. Press 'h' for help");
                break;
        }
    }
    
    delay(10);
}