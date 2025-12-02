#include "pid_controller.h"

PIDController::PIDController(float p, float i, float d) {
    kp = p;
    ki = i;
    kd = d;
    integral = 0;
    previousError = 0;
    output = 0;
    lastTime = millis();
    outputMin = -255;
    outputMax = 255;
}

void PIDController::setTunings(float p, float i, float d) {
    kp = p;
    ki = i;
    kd = d;
}

void PIDController::setOutputLimits(float min, float max) {
    outputMin = min;
    outputMax = max;
}

float PIDController::compute(float setpoint, float input) {
    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0;
    
    if(dt <= 0) return output;
    
    float error = setpoint - input;
    integral += error * dt;
    float derivative = (error - previousError) / dt;
    
    output = kp * error + ki * integral + kd * derivative;
    
    output = constrain(output, outputMin, outputMax);
    
    previousError = error;
    lastTime = currentTime;
    
    return output;
}

void PIDController::reset() {
    integral = 0;
    previousError = 0;
    output = 0;
}