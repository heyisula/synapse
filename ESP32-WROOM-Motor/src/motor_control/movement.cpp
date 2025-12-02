#include "movement.h"

MovementController::MovementController() 
    : motorDriver(nullptr), currentMovement(MOVE_STOP), 
      currentSpeed(0), maxSpeed(255), turnSpeedRatio(50) {}

void MovementController::begin(MotorDriver* driver) {
    if (driver != nullptr) {
        motorDriver = driver;
    }
    
    currentMovement = MOVE_STOP;
    currentSpeed = 0;
    
    Serial.println("Movement Controller initialized");
}

void MovementController::moveForward(int speed) {
    if (motorDriver == nullptr) return;
    
    speed = constrain(speed, 0, maxSpeed);
    
    motorDriver->setAllMotors(speed, speed);
    
    currentMovement = MOVE_FORWARD;
    currentSpeed = speed;
    
    Serial.print("Moving forward at speed: ");
    Serial.println(speed);
}

void MovementController::moveBackward(int speed) {
    if (motorDriver == nullptr) return;
    
    speed = constrain(speed, 0, maxSpeed);
    
    motorDriver->setAllMotors(-speed, -speed);
    
    currentMovement = MOVE_BACKWARD;
    currentSpeed = speed;
    
    Serial.print("Moving backward at speed: ");
    Serial.println(speed);
}

void MovementController::turnLeft(int speed, int turnRatio) {
    if (motorDriver == nullptr) return;
    
    speed = constrain(speed, 0, maxSpeed);
    turnRatio = constrain(turnRatio, 0, 100);
    
    int leftSpeed = (speed * turnRatio) / 100;
    int rightSpeed = speed;
    
    motorDriver->setAllMotors(leftSpeed, rightSpeed);
    
    currentMovement = MOVE_TURN_LEFT;
    currentSpeed = speed;
    
    Serial.print("Turning left - L:");
    Serial.print(leftSpeed);
    Serial.print(" R:");
    Serial.println(rightSpeed);
}

void MovementController::turnRight(int speed, int turnRatio) {
    if (motorDriver == nullptr) return;
    
    speed = constrain(speed, 0, maxSpeed);
    turnRatio = constrain(turnRatio, 0, 100);
    
    int leftSpeed = speed;
    int rightSpeed = (speed * turnRatio) / 100;
    
    motorDriver->setAllMotors(leftSpeed, rightSpeed);
    
    currentMovement = MOVE_TURN_RIGHT;
    currentSpeed = speed;
    
    Serial.print("Turning right - L:");
    Serial.print(leftSpeed);
    Serial.print(" R:");
    Serial.println(rightSpeed);
}

void MovementController::rotateLeft(int speed) {
    if (motorDriver == nullptr) return;
    
    speed = constrain(speed, 0, maxSpeed);
    
    // Left motors backward, right motors forward (spot turn)
    motorDriver->setAllMotors(-speed, speed);
    
    currentMovement = MOVE_ROTATE_LEFT;
    currentSpeed = speed;
    
    Serial.print("Rotating left at speed: ");
    Serial.println(speed);
}

void MovementController::rotateRight(int speed) {
    if (motorDriver == nullptr) return;
    
    speed = constrain(speed, 0, maxSpeed);
    
    // Left motors forward, right motors backward (spot turn)
    motorDriver->setAllMotors(speed, -speed);
    
    currentMovement = MOVE_ROTATE_RIGHT;
    currentSpeed = speed;
    
    Serial.print("Rotating right at speed: ");
    Serial.println(speed);
}

void MovementController::stop() {
    if (motorDriver == nullptr) return;
    
    motorDriver->stopAll();
    
    currentMovement = MOVE_STOP;
    currentSpeed = 0;
    
    Serial.println("Movement stopped");
}

void MovementController::setSpeed(int left, int right) {
    if (motorDriver == nullptr) return;
    
    left = constrain(left, -maxSpeed, maxSpeed);
    right = constrain(right, -maxSpeed, maxSpeed);
    
    motorDriver->setAllMotors(left, right);
    
    currentMovement = MOVE_CUSTOM;
    currentSpeed = max(abs(left), abs(right));
    
    Serial.print("Custom speed - L:");
    Serial.print(left);
    Serial.print(" R:");
    Serial.println(right);
}

void MovementController::arcTurn(int speed, float radius) {
    if (motorDriver == nullptr) return;
    
    // Calculate differential speeds for arc turn
    // This is a simplified implementation
    
    speed = constrain(speed, 0, maxSpeed);
    
    if (radius > 0) {
        // Turning right
        float ratio = 1.0 - (1.0 / radius);
        int leftSpeed = speed;
        int rightSpeed = speed * ratio;
        setSpeed(leftSpeed, rightSpeed);
    } else if (radius < 0) {
        // Turning left
        float ratio = 1.0 - (1.0 / abs(radius));
        int leftSpeed = speed * ratio;
        int rightSpeed = speed;
        setSpeed(leftSpeed, rightSpeed);
    } else {
        // Straight
        moveForward(speed);
    }
}