#include <Arduino.h>
#include "config/pins.h"
#include "communication/uart.h"
#include "motor/l298n.h"
#include "motor/movement.h"
#include "motor/speed.h"
#include "safety/emergency.h"

// Global objects
UARTProtocol uart;
L298NController motorController;
MovementController movementController(&motorController);
SpeedController speedController;
EmergencyStop emergencyStop(EMERGENCY_STOP_PIN, &movementController);

// Timing
unsigned long lastCommandTime = 0;
const unsigned long COMMAND_TIMEOUT = 500;  // 500ms timeout

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=================================");
    Serial.println("ESP32 WROOM - Motor Controller");
    Serial.println("=================================\n");
    
    // Initialize all systems
    uart.begin();
    motorController.begin();
    movementController.begin();
    speedController.begin();
    emergencyStop.begin();
    
    Serial.println("All systems initialized!");
    Serial.println("Waiting for commands from ESP32 S3...\n");
}

void loop() {
    // Update emergency stop system
    emergencyStop.update();
    
    // If emergency is active, don't process any movement commands
    if (emergencyStop.isEmergencyActive()) {
        // Wait for manual reset or specific command
        delay(100);
        return;
    }
    
    // Receive and process motor commands
    MotorCommand receivedCommand;
    uint8_t receivedSpeed;
    
    if (uart.receiveMotorCommand(receivedCommand, receivedSpeed)) {
        lastCommandTime = millis();
        
        // Handle emergency stop command from UART
        if (receivedCommand == CMD_EMERGENCY_STOP) {
            emergencyStop.activate();
            Serial.println("Emergency stop received via UART!");
            return;
        }
        
        // Apply speed limits
        uint8_t adjustedSpeed = speedController.applySpeedLimit(receivedSpeed);
        
        // Execute the command
        movementController.executeCommand(receivedCommand, adjustedSpeed);
        
        // Debug output
        Serial.print("Command: ");
        switch (receivedCommand) {
            case CMD_FORWARD: Serial.print("FORWARD"); break;
            case CMD_BACKWARD: Serial.print("BACKWARD"); break;
            case CMD_LEFT: Serial.print("LEFT"); break;
            case CMD_RIGHT: Serial.print("RIGHT"); break;
            case CMD_ROTATE_LEFT: Serial.print("ROTATE_LEFT"); break;
            case CMD_ROTATE_RIGHT: Serial.print("ROTATE_RIGHT"); break;
            case CMD_STOP: Serial.print("STOP"); break;
            default: Serial.print("UNKNOWN"); break;
        }
        Serial.print(" | Speed: ");
        Serial.println(adjustedSpeed);
    }
    
    // Safety timeout - stop if no command received for a while
    if ((millis() - lastCommandTime) > COMMAND_TIMEOUT && movementController.getIsMoving()) {
        Serial.println("Command timeout - stopping motors");
        movementController.stop();
    }
    
    // Small delay to prevent overwhelming the processor
    delay(10);
}