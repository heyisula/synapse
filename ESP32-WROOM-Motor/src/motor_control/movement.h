#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>
#include "motor_driver.h"

enum MovementType {
    MOVE_STOP,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_TURN_LEFT,
    MOVE_TURN_RIGHT,
    MOVE_ROTATE_LEFT,
    MOVE_ROTATE_RIGHT,
    MOVE_CUSTOM
};

class MovementController {
private:
    MotorDriver* motorDriver;
    
    MovementType currentMovement;
    int currentSpeed;
    
    // Movement parameters
    int maxSpeed;
    int turnSpeedRatio;  // Percentage of speed for turning
    
public:
    MovementController();
    void begin(MotorDriver* driver = nullptr);
    
    void setMotorDriver(MotorDriver* driver) { motorDriver = driver; }
    
    // Basic movements
    void moveForward(int speed);
    void moveBackward(int speed);
    void turnLeft(int speed, int turnRatio = 50);
    void turnRight(int speed, int turnRatio = 50);
    void rotateLeft(int speed);
    void rotateRight(int speed);
    void stop();
    
    // Advanced movements
    void setSpeed(int left, int right);
    void arcTurn(int speed, float radius);
    
    // Getters
    MovementType getCurrentMovement() { return currentMovement; }
    int getCurrentSpeed() { return currentSpeed; }
    
    void setMaxSpeed(int speed) { maxSpeed = constrain(speed, 0, 255); }
    void setTurnSpeedRatio(int ratio) { turnSpeedRatio = constrain(ratio, 0, 100); }
};

#endif