#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <Arduino.h>

class PIDController {
private:
    float kp, ki, kd;
    float integral;
    float previousError;
    float output;
    unsigned long lastTime;
    float outputMin, outputMax;
    
public:
    PIDController(float p, float i, float d);
    void setTunings(float p, float i, float d);
    void setOutputLimits(float min, float max);
    float compute(float setpoint, float input);
    void reset();
};

#endif