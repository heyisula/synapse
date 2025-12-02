#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <Arduino.h>

enum MotorID {
    MOTOR_LEFT_FRONT = 0,
    MOTOR_LEFT_BACK = 1,
    MOTOR_RIGHT_FRONT = 2,
    MOTOR_RIGHT_BACK = 3
};

struct MotorPins {
    uint8_t in1;
    uint8_t in2;
    uint8_t enable;
};

class MotorDriver {
private:
    MotorPins motors[4];
    int currentSpeeds[4];
    bool motorsEnabled;
    
    // PWM Configuration
    const int PWM_FREQ = 1000;  // 1kHz
    const int PWM_RESOLUTION = 8;  // 8-bit (0-255)
    int pwmChannels[4];  // PWM channels for each motor
    
public:
    MotorDriver();
    void begin();
    
    void setMotor(MotorID motor, int speed);
    void setAllMotors(int leftSpeed, int rightSpeed);
    void stopAll();
    void emergencyStop();
    
    void enable();
    void disable();
    bool isEnabled() { return motorsEnabled; }
    
    int getMotorSpeed(MotorID motor);
    
private:
    void setupMotorPins();
    void setMotorDirection(MotorID motor, bool forward);
    void setMotorPWM(MotorID motor, int pwmValue);
};

#endif
