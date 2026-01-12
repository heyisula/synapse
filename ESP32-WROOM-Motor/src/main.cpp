#include <Arduino.h>
#include "communication/uart.h"
#include "motor/l298n.h"
#include "motor/movement.h"
#include "motor/speed.h"
#include "safety/emergency.h"
#include "config/pins.h"
#include "config/debug.h"

// Global objects
UARTProtocol uart;
L298NController motorController;
MovementController movementController(&motorController);
SpeedController speedController;
EmergencyStop emergencyStop(EMERGENCY_STOP_PIN, &movementController);

// Timing
unsigned long lastCommandTime = 0;
const unsigned long COMMAND_TIMEOUT = 500;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    DEBUG_PRINTLN("\n=================================");
    DEBUG_PRINTLN("ESP32 WROOM - Motor Controller");
    DEBUG_PRINTLN("=================================\n");
    
    // Initialize all systems
    uart.begin();
    motorController.begin();
    movementController.begin();
    speedController.begin();
    emergencyStop.begin();
    
    DEBUG_PRINTLN("All systems initialized!");
    DEBUG_PRINTLN("Waiting for commands from ESP32 S3...\n");
}

void loop() {
    // Update emergency stop system
    emergencyStop.update();
    
    // If emergency is active, don't process any movement commands
    if (emergencyStop.isEmergencyActive()) {
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
            emergencyStop.activate(UART);
            DEBUG_PRINTLN("Emergency stop received via UART!");
            return;
        }
        
        uint8_t adjustedSpeed = speedController.applySpeedLimit(receivedSpeed);
        movementController.executeCommand(receivedCommand, adjustedSpeed);
        
        DEBUG_PRINT("Command: ");
        switch (receivedCommand) {
            case CMD_FORWARD: DEBUG_PRINT("FORWARD"); break;
            case CMD_BACKWARD: DEBUG_PRINT("BACKWARD"); break;
            case CMD_LEFT: DEBUG_PRINT("LEFT"); break;
            case CMD_RIGHT: DEBUG_PRINT("RIGHT"); break;
            case CMD_ROTATE_LEFT: DEBUG_PRINT("ROTATE_LEFT"); break;
            case CMD_ROTATE_RIGHT: DEBUG_PRINT("ROTATE_RIGHT"); break;
            case CMD_STOP: DEBUG_PRINT("STOP"); break;
            default: DEBUG_PRINT("UNKNOWN"); break;
        }
        DEBUG_PRINT(" | Speed: ");
        DEBUG_PRINTLN(adjustedSpeed);
    }
    
    // Safety timeout - stop if no command received for a while
    if ((millis() - lastCommandTime) > COMMAND_TIMEOUT && movementController.getIsMoving()) {
        DEBUG_PRINTLN("Command timeout - stopping motors");
        movementController.stop();
    }
    
    delay(10);
}