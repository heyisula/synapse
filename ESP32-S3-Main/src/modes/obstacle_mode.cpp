#include "obstacle_mode.h"
#include "../config/thresholds.h"

ObstacleMode::ObstacleMode(Ultrasonic* us, UARTProtocol* u, Buzzer* bz) {
    ultrasonic = us;
    uart = u;
    buzzer = bz;
    state = OBS_CLEAR;
    obstacleCleared = true;
}

void ObstacleMode::begin() {
    state = OBS_CLEAR;
}

void ObstacleMode::update() {
    ultrasonic->update();
    
    float frontDist = ultrasonic->getDistance(US_FRONT);
    float leftDist = ultrasonic->getDistance(US_LEFT);
    float rightDist = ultrasonic->getDistance(US_RIGHT);
    float backDist = ultrasonic->getDistance(US_BACK);
    
    if(frontDist < EMERGENCY_STOP_DISTANCE) {
        state = OBS_EMERGENCY;
        uart->sendEmergencyStop();
        buzzer->emergencyAlarm();
    } else if(frontDist < COLLISION_DISTANCE_FRONT || 
              leftDist < COLLISION_DISTANCE_SIDE || 
              rightDist < COLLISION_DISTANCE_SIDE) {
        state = OBS_DETECTED;
        handleObstacle();
    } else {
        state = OBS_CLEAR;
        obstacleCleared = true;
    }
}

void ObstacleMode::handleObstacle() {
    uart->sendMotorCommand(CMD_STOP, 0);
    delay(500);
    
    buzzer->playTone(TONE_WARNING);
    
    avoidObstacle();  // ✓ FIXED: Corrected method call
}

void ObstacleMode::avoidObstacle() {  // ✓ FIXED: Corrected method name
    float leftDist = ultrasonic->getDistance(US_LEFT);
    float rightDist = ultrasonic->getDistance(US_RIGHT);
    float rearDist = ultrasonic->getDistance(US_BACK);
    
    if(rearDist > 50) {
        // Move backward
        uart->sendMotorCommand(CMD_BACKWARD, 30);
        delay(1000);
    }
    
    if(leftDist > rightDist && leftDist > 30) {
        // Turn left
        uart->sendMotorCommand(CMD_ROTATE_LEFT, 40);
        delay(500);
    } else if(rightDist > 30) {
        // Turn right
        uart->sendMotorCommand(CMD_ROTATE_RIGHT, 40);
        delay(500);
    }
    
    uart->sendMotorCommand(CMD_STOP, 0);
}

bool ObstacleMode::isPathClear() {
    return state == OBS_CLEAR;
}