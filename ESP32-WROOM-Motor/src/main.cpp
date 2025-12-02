#include <Arduino.h>
#include "config/motor_pins.h"
#include "config/motor_config.h"
#include "motor_control/motor_driver.h"
#include "motor_control/movement.h"
#include "motor_control/speed_control.h"
#include "communication/uart_handler.h"
#include "safety/emergency_stop.h"
#include "safety/motor_protection.h"

// Global objects
MotorDriver motors;
MovementController movement;
SpeedController speedControl;
UARTHandler uart;
EmergencyStop eStop(EMERGENCY_STOP_PIN);
MotorProtection protection;

// State variables
bool emergencyActive = false;
unsigned long lastCommandTime = 0;
const unsigned long COMMAND_TIMEOUT = 1000;

// Forward declarations
void processCommand(String cmd);
void handleMoveCommand(String cmd);
void handleStopCommand();
void handleEmergencyCommand();
void handleResetCommand();
void handleStatusCommand();
void handleSpeedCommand(String cmd);
void handleTurnCommand(String cmd);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n================================");
    Serial.println("SYNAPSE Healthcare Robot");
    Serial.println("Motor Controller - ESP32-WROOM");
    Serial.println("Firmware Version 1.0");
    Serial.println("================================\n");
    
    // Initialize all modules
    Serial.println("Initializing modules...");
    
    motors.begin();
    movement.begin(&motors);
    speedControl.begin();
    eStop.begin();
    protection.begin();
    uart.begin(115200);
    
    Serial.println("\n================================");
    Serial.println("Motor Controller Ready");
    Serial.println("Waiting for commands from ESP32-S3...");
    Serial.println("================================\n");
}

void loop() {
    // Run at 100Hz
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate < 10) {
        return;
    }
    lastUpdate = millis();
    
    // Update emergency stop
    eStop.update();
    
    // Check for emergency stop button
    if (eStop.isActive()) {
        if (!emergencyActive) {
            Serial.println("\n!!! EMERGENCY STOP BUTTON PRESSED !!!");
            emergencyActive = true;
            motors.emergencyStop();
            speedControl.emergencyStop();
            uart.sendResponse("ESTOP:BUTTON_PRESSED");
        }
        return;  // Don't process any other commands
    }
    
    // Update UART handler
    uart.update();
    
    // Process incoming commands
    if (uart.hasCommand()) {
        String cmd = uart.getCommand();
        lastCommandTime = millis();
        
        Serial.print("Received command: ");
        Serial.println(cmd);
        
        processCommand(cmd);
    }
    
    // Watchdog timeout - stop if no commands received
    if (millis() - lastCommandTime > COMMAND_TIMEOUT) {
        if (speedControl.getCurrentLeftSpeed() != 0 || 
            speedControl.getCurrentRightSpeed() != 0) {
            speedControl.setTargetSpeed(0, 0);
        }
    }
    
    // Update speed control (smooth acceleration/deceleration)
    if (!emergencyActive && motors.isEnabled()) {
        speedControl.update();
        
        int leftSpeed = speedControl.getCurrentLeftSpeed();
        int rightSpeed = speedControl.getCurrentRightSpeed();
        
        motors.setAllMotors(leftSpeed, rightSpeed);
        
        // Report to protection system
        protection.reportSpeed(leftSpeed, rightSpeed);
    }
    
    // Update motor protection
    protection.update();
    
    // Check for motor faults
    if (!protection.isMotorOK() && !emergencyActive) {
        Serial.println("\n!!! MOTOR FAULT DETECTED !!!");
        emergencyActive = true;
        motors.emergencyStop();
        speedControl.emergencyStop();
        uart.sendResponse("MOTOR_FAULT");
    }
}

void processCommand(String cmd) {
    cmd.trim();
    
    if (cmd.startsWith("MOVE:")) {
        handleMoveCommand(cmd);
        
    } else if (cmd == "STOP") {
        handleStopCommand();
        
    } else if (cmd == "ESTOP") {
        handleEmergencyCommand();
        
    } else if (cmd == "RESET") {
        handleResetCommand();
        
    } else if (cmd == "STATUS") {
        handleStatusCommand();
        
    } else if (cmd.startsWith("SPEED:")) {
        handleSpeedCommand(cmd);
        
    } else if (cmd.startsWith("TURN:")) {
        handleTurnCommand(cmd);
        
    } else {
        uart.sendError("UNKNOWN_COMMAND");
        Serial.print("Unknown command: ");
        Serial.println(cmd);
    }
}

void handleMoveCommand(String cmd) {
    if (emergencyActive) {
        uart.sendError("ESTOP_ACTIVE");
        return;
    }
    
    // Parse: MOVE:leftSpeed,rightSpeed
    int commaIndex = cmd.indexOf(',');
    if (commaIndex > 0) {
        int leftSpeed = cmd.substring(5, commaIndex).toInt();
        int rightSpeed = cmd.substring(commaIndex + 1).toInt();
        
        speedControl.setTargetSpeed(leftSpeed, rightSpeed);
        uart.sendOK();
        
        Serial.print("Moving - L:");
        Serial.print(leftSpeed);
        Serial.print(" R:");
        Serial.println(rightSpeed);
    } else {
        uart.sendError("INVALID_FORMAT");
    }
}

void handleStopCommand() {
    speedControl.setTargetSpeed(0, 0);
    uart.sendOK();
    Serial.println("Stop command received");
}

void handleEmergencyCommand() {
    emergencyActive = true;
    motors.emergencyStop();
    speedControl.emergencyStop();
    uart.sendResponse("ESTOP:ACTIVE");
    Serial.println("Emergency stop triggered via UART");
}

void handleResetCommand() {
    if (eStop.isPressed()) {
        uart.sendError("BUTTON_STILL_PRESSED");
        Serial.println("Reset failed - button still pressed");
    } else {
        emergencyActive = false;
        motors.enable();
        protection.resetFaults();
        eStop.reset();
        uart.sendResponse("OK:RESET");
        Serial.println("System reset successful");
    }
}

void handleStatusCommand() {
    String status = "STATUS:";
    status += "ESTOP=" + String(emergencyActive ? "1" : "0");
    status += ",MOTORS=" + String(protection.isMotorOK() ? "OK" : "FAULT");
    status += ",L=" + String(speedControl.getCurrentLeftSpeed());
    status += ",R=" + String(speedControl.getCurrentRightSpeed());
    status += ",ENABLED=" + String(motors.isEnabled() ? "1" : "0");
    
    uart.sendResponse(status);
    Serial.println(status);
}

void handleSpeedCommand(String cmd) {
    // SPEED:value (set max acceleration)
    int speed = cmd.substring(6).toInt();
    speedControl.setAccelerationLimit(speed);
    uart.sendOK();
    Serial.print("Speed limit set to: ");
    Serial.println(speed);
}

void handleTurnCommand(String cmd) {
    if (emergencyActive) {
        uart.sendError("ESTOP_ACTIVE");
        return;
    }
    
    // TURN:direction,speed
    // direction: L=left, R=right
    // Example: TURN:L,150
    
    int commaIndex = cmd.indexOf(',');
    if (commaIndex > 0) {
        String direction = cmd.substring(5, commaIndex);
        int speed = cmd.substring(commaIndex + 1).toInt();
        
        if (direction == "L") {
            movement.rotateLeft(speed);
        } else if (direction == "R") {
            movement.rotateRight(speed);
        }
        
        uart.sendOK();
    } else {
        uart.sendError("INVALID_FORMAT");
    }
}