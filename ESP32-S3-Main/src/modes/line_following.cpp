#include "line_following.h"

LineFollowing::LineFollowing(BFD1000* s, UARTProtocol* u, Display* d) 
    : sensor(s), uart(u), display(d), isActive(false), baseSpeed(MOTOR_SPEED_DEFAULT), lastError(0) {}

void LineFollowing::begin() {
    sensor->begin();
}

void LineFollowing::start() {
    isActive = true;
    display->clear();
    display->setCursor(0, 0);
    display->print("Line Following");
    display->setCursor(0, 1);
    display->print("Mode: ACTIVE");
}

void LineFollowing::stop() {
    isActive = false;
    uart->sendMotorCommand(CMD_STOP, 0);
}

void LineFollowing::update() {
    if (!isActive) return;

    sensor->update();
    updateSteering();
}

float LineFollowing::calculateError() {
    float error = 0;
    int count = 0;

    if (sensor->isBlack(0)) { error -= 2; count++; }// Leftmost
    if (sensor->isBlack(1)) { error -= 1; count++; }// Left
    if (sensor->isBlack(2)) { error += 0; count++; } // Center
    if (sensor->isBlack(3)) { error += 1; count++; }// Right
    if (sensor->isBlack(4)) { error += 2; count++; }// Rightmost

    if (count == 0) return lastError; // Keep last error if line lost
    
    return error / count;
}

void LineFollowing::updateSteering() {
    float error = calculateError();
    float dError = error - lastError;
    lastError = error;

    float steering = (error * KP) + (dError * KD);

    if (abs(error) < 0.5f) {
        uart->sendMotorCommand(CMD_FORWARD, baseSpeed);
    } else if (steering > 0) {
        // Line is to the right, turn right
        uart->sendMotorCommand(CMD_RIGHT, baseSpeed);
    } else {
        // Line is to the left, turn left
        uart->sendMotorCommand(CMD_LEFT, baseSpeed);
    }
}

bool LineFollowing::isLineDetected() {
    return sensor->isAnyBlack();
}
