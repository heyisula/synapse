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

unsigned long lastCommandTime = 0;
unsigned long commandCount = 0;
unsigned long errorCount = 0;

void printDiagnostics() {
    Serial.println("\n========== SYSTEM DIAGNOSTICS ==========");
    Serial.printf("Uptime: %lu ms\n", millis());
    Serial.printf("Commands Received: %lu\n", commandCount);
    Serial.printf("Errors: %lu\n", errorCount);
    Serial.printf("Last Command: %lu ms ago\n", millis() - lastCommandTime);
    
    Serial.println("\n--- Motor Status ---");
    Serial.printf("Current Command: ");
    switch(movementController.getCurrentCommand()) {
        case CMD_STOP: Serial.println("STOP"); break;
        case CMD_FORWARD: Serial.println("FORWARD"); break;
        case CMD_BACKWARD: Serial.println("BACKWARD"); break;
        case CMD_LEFT: Serial.println("LEFT"); break;
        case CMD_RIGHT: Serial.println("RIGHT"); break;
        case CMD_ROTATE_LEFT: Serial.println("ROTATE_LEFT"); break;
        case CMD_ROTATE_RIGHT: Serial.println("ROTATE_RIGHT"); break;
        case CMD_EMERGENCY_STOP: Serial.println("EMERGENCY_STOP"); break;
        default: Serial.println("UNKNOWN"); break;
    }
    Serial.printf("Current Speed: %d%%\n", movementController.getCurrentSpeed());
    Serial.printf("Is Moving: %s\n", movementController.getIsMoving() ? "Yes" : "No");
    
    Serial.println("\n--- Safety Status ---");
    Serial.printf("Emergency Active: %s\n", emergencyStop.isEmergencyActive() ? "YES ⚠️" : "No");
    Serial.printf("Emergency Button: %s\n", 
                  digitalRead(EMERGENCY_STOP_PIN) == LOW ? "PRESSED 🔴" : "Released 🟢");
    
    Serial.println("\n--- Speed Controller ---");
    Serial.printf("Min Speed: %d%%\n", speedController.getMinSpeed());
    Serial.printf("Max Speed: %d%%\n", speedController.getMaxSpeed());
    
    Serial.println("\n--- Hardware Status ---");
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
    
    Serial.println("=========================================\n");
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n========================================");
    Serial.println("   System Diagnostic Monitor");
    Serial.println("========================================\n");
    
    uart.begin();
    motorController.begin();
    movementController.begin();
    speedController.begin();
    emergencyStop.begin();
    
    Serial.println("✓ All systems online");
    Serial.println("Monitoring system status...\n");
}

void loop() {
    emergencyStop.update();
    
    // Receive UART commands
    MotorCommand cmd;
    uint8_t speed;
    
    if (uart.receiveMotorCommand(cmd, speed)) {
        lastCommandTime = millis();
        commandCount++;
        
        if (cmd == CMD_EMERGENCY_STOP) {
            emergencyStop.activate();
        } else if (!emergencyStop.isEmergencyActive()) {
            uint8_t adjustedSpeed = speedController.applySpeedLimit(speed);
            movementController.executeCommand(cmd, adjustedSpeed);
        }
    }
    
    // Print diagnostics every 5 seconds
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 5000) {
        lastPrint = millis();
        printDiagnostics();
    }
    
    // Check for timeout
    if ((millis() - lastCommandTime) > 500 && movementController.getIsMoving()) {
        movementController.stop();
        Serial.println("⚠️ Command timeout - motors stopped");
    }
    
    delay(10);
}
