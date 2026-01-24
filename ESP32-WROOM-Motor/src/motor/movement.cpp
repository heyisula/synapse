#include "movement.h"
#include "config/debug.h"

MovementController::MovementController(L298NController* motors) {
    motorController = motors;
    currentCommand = CMD_STOP;
    currentSpeed = 0;
    isMoving = false;
}

void MovementController::begin() {
    if (motorController == nullptr) {
        DEBUG_PRINTLN("ERROR: Motor controller is null!");
        return;
    }

    motorController->begin();
    stop();
}

void MovementController::executeCommand(MotorCommand cmd, uint8_t speed) {
    if (motorController == nullptr) {
        DEBUG_PRINTLN("ERROR: Motor controller is null!");
        return;
    }

    currentCommand = cmd;
    currentSpeed = constrain(speed, 0, 100);
    
    switch (cmd) {
        case CMD_FORWARD:
            moveForward(currentSpeed);
            isMoving = true;
            break;
            
        case CMD_BACKWARD:
            moveBackward(currentSpeed);
            isMoving = true;
            break;
            
        case CMD_LEFT:
            turnLeft(currentSpeed);
            isMoving = true;
            break;
            
        case CMD_RIGHT:
            turnRight(currentSpeed);
            isMoving = true;
            break;
            
        case CMD_ROTATE_LEFT:
            rotateLeft(currentSpeed);
            isMoving = true;
            break;
            
        case CMD_ROTATE_RIGHT:
            rotateRight(currentSpeed);
            isMoving = true;
            break;
            
        case CMD_STRAFE_LEFT:
            strafeLeft(currentSpeed);
            isMoving = true;
            break;
            
        case CMD_STRAFE_RIGHT:
            strafeRight(currentSpeed);
            isMoving = true;
            break;
            
        case CMD_STOP:
            stop();
            break;
            
        case CMD_EMERGENCY_STOP:
            emergencyStop();
            break;
            
        default:
            stop();
            break;
    }
}

void MovementController::stop() {
    if (motorController == nullptr) return;
    
    motorController->allStop();
    currentCommand = CMD_STOP;
    currentSpeed = 0;
    isMoving = false;
}

void MovementController::emergencyStop() {
    if (motorController == nullptr) return;
    
    motorController->allBrake();
    currentCommand = CMD_EMERGENCY_STOP;
    currentSpeed = 0;
    isMoving = false;
}

void MovementController::moveForward(uint8_t speed) {
    if (motorController == nullptr) return;
    motorController->allForward(speed);
}

void MovementController::moveBackward(uint8_t speed) {
    if (motorController == nullptr) return;
    motorController->allBackward(speed);
}

void MovementController::turnLeft(uint8_t speed) {
    if (motorController == nullptr) return;
    
    // Turn left by slowing down left motors - using integer math
    uint8_t reducedSpeed = (speed * TURN_SPEED_RATIO) / 100;  // Left side at 30% speed
    motorController->leftSideForward(reducedSpeed);
    motorController->rightSideForward(speed);
}

void MovementController::turnRight(uint8_t speed) {
    if (motorController == nullptr) return;
    
    // Turn right by slowing down right motors - using integer math
    uint8_t reducedSpeed = (speed * TURN_SPEED_RATIO) / 100;  // Right side at 30% speed
    motorController->leftSideForward(speed);
    motorController->rightSideForward(reducedSpeed);
}

void MovementController::rotateLeft(uint8_t speed) {
    if (motorController == nullptr) return;
    
    // Rotate in place - left motors backward, right motors forward
    motorController->leftSideBackward(speed);
    motorController->rightSideForward(speed);
}

void MovementController::rotateRight(uint8_t speed) {
    if (motorController == nullptr) return;
    
    // Rotate in place - left motors forward, right motors backward
    motorController->leftSideForward(speed);
    motorController->rightSideBackward(speed);
}

void MovementController::strafeLeft(uint8_t speed) {
    if (motorController == nullptr) return;
    motorController->strafeLeft(speed);
}

void MovementController::strafeRight(uint8_t speed) {
    if (motorController == nullptr) return;
    motorController->strafeRight(speed);
}

MotorCommand MovementController::getCurrentCommand() {
    return currentCommand;
}

uint8_t MovementController::getCurrentSpeed() {
    return currentSpeed;
}

bool MovementController::getIsMoving() {
    return isMoving;
}