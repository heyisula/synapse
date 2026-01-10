#ifndef L298N_H
#define L298N_H

#include <Arduino.h>

class L298NMotor {
private:
    uint8_t in1Pin;
    uint8_t in2Pin;
    uint8_t enPin;
    uint8_t currentSpeed;
    bool isEnabled;
    
    const uint8_t PWM_CHANNEL;
    const uint32_t PWM_FREQ = 5000;
    const uint8_t PWM_RESOLUTION = 8;

public:
    L298NMotor(uint8_t in1, uint8_t in2, uint8_t en, uint8_t pwmChannel);
    void begin();
    void forward(uint8_t speed);
    void backward(uint8_t speed);
    void stop();
    void brake();
    uint8_t getCurrentSpeed();
    void setSpeed(uint8_t speed);
};

class L298NController {
private:
    L298NMotor* leftFrontMotor;
    L298NMotor* leftBackMotor;
    L298NMotor* rightFrontMotor;
    L298NMotor* rightBackMotor;

public:
    L298NController();
    void begin();
    void allForward(uint8_t speed);
    void allBackward(uint8_t speed);
    void allStop();
    void allBrake();
    
    // Individual motor control
    void leftSideForward(uint8_t speed);
    void leftSideBackward(uint8_t speed);
    void rightSideForward(uint8_t speed);
    void rightSideBackward(uint8_t speed);
    
    void leftSideStop();
    void rightSideStop();
    
    L298NMotor* getLeftFront() { return leftFrontMotor; }
    L298NMotor* getLeftBack() { return leftBackMotor; }
    L298NMotor* getRightFront() { return rightFrontMotor; }
    L298NMotor* getRightBack() { return rightBackMotor; }
};

#endif