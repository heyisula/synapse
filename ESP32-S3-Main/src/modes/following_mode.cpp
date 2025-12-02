#include "following_mode.h"
#include "../config/constants.h"

FollowingMode::FollowingMode(Ultrasonic* us, ColorSensor* cs, UARTProtocol* u) {
    ultrasonic = us;
    colorSensor = cs;
    uart = u;
    state = FOLLOW_IDLE;
    targetDistance = FOLLOW_DISTANCE_DEFAULT;
    targetMoving = false;
    lastDetectionTime = 0;
    priority = 3;
}

void FollowingMode::begin() {
    state = FOLLOW_SCANNING;
}

void FollowingMode::update() {
    float frontDistance = ultrasonic->getDistance(US_FRONT);
    
    switch(state) {
        case FOLLOW_IDLE:
            if(isHumanDetected()) {
                state = FOLLOW_TRACKING;
                identifyStaff();
            }
            break;
            
        case FOLLOW_SCANNING:
            if(isHumanDetected()) {
                state = FOLLOW_TRACKING;
                identifyStaff();
                lastDetectionTime = millis();
            }
            break;
            
        case FOLLOW_TRACKING:
            if(frontDistance > targetDistance + 50) {
                // Move forward
                uart->sendMotorCommand(CMD_FORWARD, 40);
            } else if(frontDistance < targetDistance - 20) {
                // Move backward
                uart->sendMotorCommand(CMD_BACKWARD, 30);
            } else {
                // Maintain position
                uart->sendMotorCommand(CMD_STOP, 0);
            }
            
            // Check if target is stationary
            if(millis() - lastDetectionTime > 30000) {
                state = FOLLOW_STANDBY;
            }
            break;
            
        case FOLLOW_STANDBY:
            uart->sendMotorCommand(CMD_STOP, 0);
            if(isHumanDetected() && (millis() - lastDetectionTime < 1000)) {
                state = FOLLOW_TRACKING;
            }
            break;
    }
}

void FollowingMode::setTargetDistance(float distance) {
    targetDistance = constrain(distance, FOLLOW_DISTANCE_MIN, FOLLOW_DISTANCE_MAX);
}

bool FollowingMode::isHumanDetected() {
    float distance = ultrasonic->getDistance(US_FRONT);
    return distance >= 50 && distance <= 200;
}

void FollowingMode::identifyStaff() {
    colorSensor->update();
    ColorType color = colorSensor->getColorType();
    
    if(color == COLOR_WHITE) {
        priority = 1; // Doctor (white coat)
    } else if(color == COLOR_BLUE) {
        priority = 2; // Nurse (blue scrubs)
    } else {
        priority = 3; // Unknown
    }
}

void FollowingMode::setPriority(int p) {
    priority = constrain(p, 1, 3);
}

int FollowingMode::getPriority() {
    return priority;
}