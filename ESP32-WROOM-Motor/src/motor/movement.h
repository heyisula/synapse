#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>
#include "l298n.h"
#include "communication/uart.h"

// Turn speed ratio constant (percentage)
#define TURN_SPEED_RATIO 30

class MovementController {
private:
    L298NController* motorController;
    MotorCommand currentCommand;
    uint8_t currentSpeed;
    bool isMoving;

public:
    MovementController(L298NController* motors);
    void begin();
    void executeCommand(MotorCommand cmd, uint8_t speed);
    void stop();
    void emergencyStop();
    
    MotorCommand getCurrentCommand();
    uint8_t getCurrentSpeed();
    bool getIsMoving();

private:
    void moveForward(uint8_t speed);
    void moveBackward(uint8_t speed);
    void turnLeft(uint8_t speed);
    void turnRight(uint8_t speed);
    void rotateLeft(uint8_t speed);
    void rotateRight(uint8_t speed);
};

#endif