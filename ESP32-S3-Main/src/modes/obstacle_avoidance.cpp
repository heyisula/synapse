#include "obstacle_avoidance.h"

ObstacleAvoidance::ObstacleAvoidance(UltrasonicManager* us, MotionTracker* mt, 
                                     UARTProtocol* u, Display* d, Buzzer* b) {
    ultrasonicMgr = us;
    motionTracker = mt;
    uart = u;
    display = d;
    buzzer = b;

    currentStatus = STATUS_CLEAR;
    currentSpeed = MOTOR_SPEED_DEFAULT;
    isMoving = true;
    stationaryCount = 0;

    frontDistance = MAX_ULTRASONIC_DISTANCE;
    rearDistance = MAX_ULTRASONIC_DISTANCE;
    leftDistance = MAX_ULTRASONIC_DISTANCE;
    rightDistance = MAX_ULTRASONIC_DISTANCE;

    gyroX = gyroY = 0.0f;
    accelX = accelY = 0.0f;
    pitch = roll = 0.0f;
}

void ObstacleAvoidance::begin() {
    // Initialize all sensors and peripherals
    ultrasonicMgr->begin();
    motionTracker->begin();
    motionTracker->autoCalibrate();
    uart->begin();
    display->begin();
    buzzer->begin();

    display->clear();
    display->setCursor(0, 0);
    display->print("Obstacle Avoid");
    display->setCursor(0, 1);
    display->print("System Ready");
    delay(1000);
}

void ObstacleAvoidance::update() {
    //Read all sensor values
    ultrasonicMgr->update();
    motionTracker->update();
    buzzer->update(); 
    
    // Update internal distance caches
    updateSensorReadings();

    //Decision Making & UART Signaling
    //1: Emergency Stop
    if (frontDistance < EMERGENCY_STOP_DISTANCE) {
        currentStatus = STATUS_STOP;
        currentSpeed = 0;
        uart->sendEmergencyStop();
        buzzer->playTone(TONE_ERROR);
        displaySafetyStatus(STATUS_STOP);
        return;
    }

    //2: Safe Reversing
    if (rearDistance < COLLISION_DISTANCE_BACK) {
        currentStatus = STATUS_DONT_REVERSE;
    } else if (currentStatus == STATUS_DONT_REVERSE) {
        currentStatus = STATUS_CLEAR;
    }

    //3: Autonomous Pathfinding
    if (frontDistance < COLLISION_DISTANCE_FRONT) {
        // Path is partially blocked ahead
        currentStatus = STATUS_SLOW;
        currentSpeed = MOTOR_SPEED_MIN;

        // Decide which way to turn
        if (leftDistance > rightDistance && leftDistance > COLLISION_DISTANCE_SIDE) {
            // Left is clearer - Rotate Left
            uart->sendMotorCommand(CMD_ROTATE_LEFT, MOTOR_SPEED_MIN);
            Serial.println("⟲ Path blocked - Rotating Left");
        } 
        else if (rightDistance > leftDistance && rightDistance > COLLISION_DISTANCE_SIDE) {
            // Right is clearer - Rotate Right
            uart->sendMotorCommand(CMD_ROTATE_RIGHT, MOTOR_SPEED_MIN);
            Serial.println("⟳ Path blocked - Rotating Right");
        }
        else {
            // All paths ahead/sides blocked - Reverse
            if (canReverse()) {
                uart->sendMotorCommand(CMD_BACKWARD, MOTOR_SPEED_MIN);
                Serial.println("↓ Dead end - Reversing");
            } else {
                uart->sendEmergencyStop();
                Serial.println("⚠ Fully trapped! Emergency Stop.");
            }
        }
    } 
    else {
        // Path is clear - Drive Forward
        currentStatus = STATUS_CLEAR;
        currentSpeed = MOTOR_SPEED_DEFAULT;
        uart->sendMotorCommand(CMD_FORWARD, currentSpeed);
        Serial.print("↑ Roaming Forward - Speed: ");
        Serial.println(currentSpeed);
    }

    //4: Handle Tilt/Gyro
    if (checkGyroAccelThreshold()) {
        // Override speed if unbalanced
        currentSpeed = MOTOR_SPEED_MIN;
        displaySafetyStatus(STATUS_SLOW);
    }

    // Update Display periodically
    static unsigned long lastDisp = 0;
    if (millis() - lastDisp > 500) {
        displaySafetyStatus(currentStatus);
        lastDisp = millis();
    }
}

void ObstacleAvoidance::updateSensorReadings() {
    // Distance readings
    frontDistance = round(ultrasonicMgr->getDistance(US_FRONT));
    rearDistance = round(ultrasonicMgr->getDistance(US_BACK));
    leftDistance = round(ultrasonicMgr->getDistance(US_LEFT));
    rightDistance = round(ultrasonicMgr->getDistance(US_RIGHT));

    // MPU6050 readings)
    pitch = round(motionTracker->getPitch());  // Left/Right tilt
    roll = round(motionTracker->getRoll() )+4;    // Front/Back tilt (Calibrated +3 deg)
    accelX = round(motionTracker->getForwardAcceleration());
    accelY = round(motionTracker->getSideAcceleration());
}

bool ObstacleAvoidance::checkFrontDistance() {
    // Check if front obstacle is within emergency distance
    return (frontDistance < EMERGENCY_STOP_DISTANCE);
}

bool ObstacleAvoidance::checkRearDistance() {
    // Check if rear obstacle is within threshold
    return (rearDistance < COLLISION_DISTANCE_BACK);
}

bool ObstacleAvoidance::checkGyroAccelThreshold() {
    // Check if tilt angles exceed safe limits
    if (fabs(roll) > STATIONARY_TILT_THRESHOLD) {
        return true;
    }

    // Check if acceleration is excessive (sudden impact/collision)
    // Note: AccelX (Forward/Back) removed due to noise
    if (fabs(accelY) > 0.5f) {
        return true;
    }

    return false;
}

bool ObstacleAvoidance::checkIfStationary() {
    // Check if robot is stationary based on gyro and tilt
    if (fabs(roll) < STATIONARY_GYRO_THRESHOLD &&
        fabs(accelY) < 0.1f) {
        stationaryCount++;
        if (stationaryCount >= STATIONARY_COUNT_THRESHOLD) {
            isMoving = false;
            return true;
        }
    } else {
        stationaryCount = 0;
        isMoving = true;
    }
    return false;
}

void ObstacleAvoidance::handleEmergencyStop() {
    currentStatus = STATUS_EMERGENCY;
    currentSpeed = 0;

    //Emergency Stop - send via UART using proper protocol
    uart->sendEmergencyStop();

    //Activate Audio Alert
    buzzer->emergencyAlarm();

    //Display Safety Status: Stop
    displaySafetyStatus(STATUS_STOP);
}

void ObstacleAvoidance::handleRearObstacle() {
    currentStatus = STATUS_DONT_REVERSE;

    displaySafetyStatus(STATUS_DONT_REVERSE);
}

void ObstacleAvoidance::handleGyroAccelExceeded() {
    currentStatus = STATUS_SLOW;

    // Reduce speed and send slow forward command via UART
    currentSpeed = MOTOR_SPEED_MIN;
    uart->sendMotorCommand(CMD_FORWARD, MOTOR_SPEED_MIN);

    // Display Safety Status: Slow
    displaySafetyStatus(STATUS_SLOW);
}

void ObstacleAvoidance::displaySafetyStatus(SafetyStatus status) {
    display->clear();

    switch (status) {
        case STATUS_CLEAR:
            display->setCursor(0, 0);
            display->print("Status: CLEAR");
            display->setCursor(0, 1);
            display->print("F:");
            display->print(frontDistance, 1);
            display->print(" R:");
            display->print(rearDistance, 1);
            display->setCursor(0, 2);
            display->print("L:");
            display->print(leftDistance, 1);
            display->print(" R:");
            display->print(rightDistance, 1);
            display->setCursor(0, 3);
            display->print("Speed: ");
            display->print(currentSpeed);
            display->print("%");
            break;

        case STATUS_SLOW:
            display->setCursor(0, 0);
            display->print("Status: SLOW");
            display->setCursor(0, 1);
            display->print("Tilt Detected!");
            display->setCursor(0, 2);
            display->print("Pitch:");
            display->print(pitch, 1);
            display->print(" Roll:");
            display->print(roll, 1);
            display->setCursor(0, 3);
            display->print("Reducing Speed");
            break;

        case STATUS_DONT_REVERSE:
            display->setCursor(0, 0);
            display->print("Status: CAUTION");
            display->setCursor(0, 1);
            display->print("Rear Obstacle!");
            display->setCursor(0, 2);
            display->print("Distance: ");
            display->print(rearDistance, 1);
            display->print("cm");
            display->setCursor(0, 3);
            display->print("NO REVERSE!");
            break;

        case STATUS_STOP:
            display->setCursor(0, 0);
            display->print("***EMERGENCY***");
            display->setCursor(0, 1);
            display->print("OBSTACLE AHEAD!");
            display->setCursor(0, 2);
            display->print("Distance: ");
            display->print(frontDistance, 1);
            display->print("cm");
            display->setCursor(0, 3);
            display->print("**SYSTEM STOP**");
            break;

        case STATUS_EMERGENCY:
            display->setCursor(0, 0);
            display->print("!!!EMERGENCY!!!");
            display->setCursor(0, 1);
            display->print("COLLISION RISK!");
            display->setCursor(0, 2);
            display->print("Front: ");
            display->print(frontDistance, 1);
            display->print("cm");
            display->setCursor(0, 3);
            display->print("STOPPED!");
            break;
    }
}

void ObstacleAvoidance::updateMovementPermissions() {
    switch (currentStatus) {
        case STATUS_CLEAR:
            break;
        case STATUS_SLOW:
            break;
        case STATUS_DONT_REVERSE:
            break;
        case STATUS_STOP:
        case STATUS_EMERGENCY:
            uart->sendEmergencyStop();
            break;
    }
}

SafetyStatus ObstacleAvoidance::getSafetyStatus() {
    return currentStatus;
}

ObstacleAvoidance::SensorData ObstacleAvoidance::getSensorData() {
    return lastSensorData;
}

bool ObstacleAvoidance::canMove() {
    return (currentStatus != STATUS_STOP && 
            currentStatus != STATUS_EMERGENCY);
}

bool ObstacleAvoidance::canReverse() {
    return (currentStatus != STATUS_DONT_REVERSE &&
            currentStatus != STATUS_STOP &&
            currentStatus != STATUS_EMERGENCY);
}

uint8_t ObstacleAvoidance::getRecommendedSpeed() {
    return currentSpeed;
}