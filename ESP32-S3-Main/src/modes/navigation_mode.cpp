#include "navigation_mode.h"
#include "../config/constants.h"

NavigationMode::NavigationMode(LineFollower* lf, Ultrasonic* us, MotionTracker* mt, UARTProtocol* u) {
    lineFollower = lf;
    ultrasonic = us;
    imu = mt;
    uart = u;
    linePID = new PIDController(2.0, 0.0, 1.0);
    linePID->setOutputLimits(-50, 50);
    state = NAV_IDLE;
    deviation = 0;
    currentWaypoint = {0, "", false};
}

void NavigationMode::begin() {
    state = NAV_FOLLOWING_LINE;
}

void NavigationMode::update() {
    lineFollower->update();
    
    switch(state) {
        case NAV_IDLE:
            uart->sendMotorCommand(CMD_STOP, 0);
            break;
            
        case NAV_FOLLOWING_LINE:
            if(lineFollower->isLineDetected()) {
                followLine();
            } else {
                state = NAV_PATH_LOST;
            }
            break;
            
        case NAV_AT_DESTINATION:
            uart->sendMotorCommand(CMD_STOP, 0);
            currentWaypoint.reached = true;
            break;
            
        case NAV_PATH_LOST:
            handlePathLost();
            break;
    }
}

void NavigationMode::loadDestination(String location) {
    currentWaypoint.location = location;
    currentWaypoint.reached = false;
    state = NAV_FOLLOWING_LINE;
}

bool NavigationMode::isDestinationReached() {
    return currentWaypoint.reached;
}

void NavigationMode::followLine() {
    int error = lineFollower->getError();
    float correction = linePID->compute(0, error);
    
    int baseSpeed = 50;
    int leftSpeed = baseSpeed + correction;
    int rightSpeed = baseSpeed - correction;
    
    leftSpeed = constrain(leftSpeed, 0, 100);
    rightSpeed = constrain(rightSpeed, 0, 100);
    
    // Send motor command (simplified)
    if(correction > 5) {
        uart->sendMotorCommand(CMD_LEFT, abs(correction));
    } else if(correction < -5) {
        uart->sendMotorCommand(CMD_RIGHT, abs(correction));
    } else {
        uart->sendMotorCommand(CMD_FORWARD, baseSpeed);
    }
}

void NavigationMode::handlePathLost() {
    uart->sendMotorCommand(CMD_STOP, 0);
    delay(1000);
    
    // Search for line
    for(int i = 0; i < 4; i++) {
        uart->sendMotorCommand(CMD_ROTATE_LEFT, 30);
        delay(500);
        lineFollower->update();
        if(lineFollower->isLineDetected()) {
            state = NAV_FOLLOWING_LINE;
            return;
        }
    }
    
    // Emergency navigation mode
    state = NAV_PATH_LOST;
}