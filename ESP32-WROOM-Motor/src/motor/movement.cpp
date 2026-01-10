#include "movement.h"

MovementController::MovementController(L298NController* motors) {
    motorController = motors;
    currentCommand = CMD_STOP;
    currentSpeed = 0;
    isMoving = false;
}

void MovementController::begin() {
    motorController->begin();
    stop();
}

void MovementController::executeCommand(MotorCommand cmd, uint8_t speed) {
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
    motorController->allStop();
    currentCommand = CMD_STOP;
    currentSpeed = 0;
    isMoving = false;
}

void MovementController::emergencyStop() {
    motorController->allBrake();
    currentCommand = CMD_EMERGENCY_STOP;
    currentSpeed = 0;
    isMoving = false;
}

void MovementController::moveForward(uint8_t speed) {
    motorController->allForward(speed);
}

void MovementController::moveBackward(uint8_t speed) {
    motorController->allBackward(speed);
}

void MovementController::turnLeft(uint8_t speed) {
    // Turn left by slowing down left motors
    uint8_t reducedSpeed = speed * 0.3;  // Left side at 30% speed
    motorController->leftSideForward(reducedSpeed);
    motorController->rightSideForward(speed);
}

void MovementController::turnRight(uint8_t speed) {
    // Turn right by slowing down right motors
    uint8_t reducedSpeed = speed * 0.3;  // Right side at 30% speed
    motorController->leftSideForward(speed);
    motorController->rightSideForward(reducedSpeed);
}

void MovementController::rotateLeft(uint8_t speed) {
    // Rotate in place - left motors backward, right motors forward
    motorController->leftSideBackward(speed);
    motorController->rightSideForward(speed);
}

void MovementController::rotateRight(uint8_t speed) {
    // Rotate in place - left motors forward, right motors backward
    motorController->leftSideForward(speed);
    motorController->rightSideBackward(speed);
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