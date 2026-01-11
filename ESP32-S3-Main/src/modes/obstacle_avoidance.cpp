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
    // Step 1: Read all sensor values
    updateSensorReadings();

    // Step 2: Check front distance first (highest priority)
    if (checkFrontDistance()) {
        handleEmergencyStop();
        return;  // Exit immediately on emergency
    }

    // Step 3: Check rear distance
    if (checkRearDistance()) {
        handleRearObstacle();
    } else {
        // Clear if no rear obstacle
        if (currentStatus == STATUS_DONT_REVERSE) {
            currentStatus = STATUS_CLEAR;
            displaySafetyStatus(STATUS_CLEAR);
        }
    }

    // Step 4: Check gyro/accel thresholds
    if (checkGyroAccelThreshold()) {
        handleGyroAccelExceeded();
    } else {
        // Clear slow status if gyro/accel normal
        if (currentStatus == STATUS_SLOW) {
            currentStatus = STATUS_CLEAR;
            displaySafetyStatus(STATUS_CLEAR);
        }
    }

    // Step 5: Update movement permissions
    updateMovementPermissions();

    // Step 6: Provide data for API (logging/telemetry)
    lastSensorData.frontDist = frontDistance;
    lastSensorData.rearDist = rearDistance;
    lastSensorData.leftDist = leftDistance;
    lastSensorData.rightDist = rightDistance;
    lastSensorData.pitch = pitch;
    lastSensorData.roll = roll;
    lastSensorData.forwardAccel = accelX;
    lastSensorData.sideAccel = accelY;
    lastSensorData.status = currentStatus;
    lastSensorData.speed = currentSpeed;

    // Wait 100ms before next iteration (as per flowchart)
    delay(100);
}

void ObstacleAvoidance::updateSensorReadings() {
    // Update ultrasonic sensors
    ultrasonicMgr->update();
    frontDistance = ultrasonicMgr->getDistance(US_FRONT);
    rearDistance = ultrasonicMgr->getDistance(US_BACK);
    leftDistance = ultrasonicMgr->getDistance(US_LEFT);
    rightDistance = ultrasonicMgr->getDistance(US_RIGHT);

    // Update MPU6050
    motionTracker->update();
    pitch = motionTracker->getPitch();  // Left/Right tilt
    roll = motionTracker->getRoll();    // Front/Back tilt
    accelX = motionTracker->getForwardAcceleration();
    accelY = motionTracker->getSideAcceleration();
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
    if (fabs(pitch) > STATIONARY_TILT_THRESHOLD || 
        fabs(roll) > STATIONARY_TILT_THRESHOLD) {
        return true;
    }

    // Check if acceleration is excessive (sudden impact/collision)
    if (fabs(accelX) > 0.5f || fabs(accelY) > 0.5f) {
        return true;
    }

    return false;
}

bool ObstacleAvoidance::checkIfStationary() {
    // Check if robot is stationary based on gyro and tilt
    if (fabs(pitch) < STATIONARY_GYRO_THRESHOLD && 
        fabs(roll) < STATIONARY_GYRO_THRESHOLD &&
        fabs(accelX) < 0.1f && 
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

    // Step 1: Emergency Stop
    uart->sendEmergencyStop();

    // Step 2: Activate Audio Alert
    buzzer->emergencyAlarm();

    // Step 3: Display Safety Status: Stop
    displaySafetyStatus(STATUS_STOP);
}

void ObstacleAvoidance::handleRearObstacle() {
    currentStatus = STATUS_DONT_REVERSE;

    // Disable moving reverse
    // This will be checked by motor controller

    // Display Safety Status: Don't Reverse
    displaySafetyStatus(STATUS_DONT_REVERSE);
}

void ObstacleAvoidance::handleGyroAccelExceeded() {
    currentStatus = STATUS_SLOW;

    // Reduce speed
    currentSpeed = MOTOR_SPEED_MIN;

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
    // This function updates what movements are allowed
    // based on current safety status
    
    // Send motor commands based on status
    switch (currentStatus) {
        case STATUS_CLEAR:
            // Normal operation - movements allowed
            break;

        case STATUS_SLOW:
            // Reduced speed but can move
            uart->sendMotorCommand(CMD_FORWARD, MOTOR_SPEED_MIN);
            break;

        case STATUS_DONT_REVERSE:
            // Can't reverse, but forward ok
            // Motor controller should handle this
            break;

        case STATUS_STOP:
        case STATUS_EMERGENCY:
            // Complete stop
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