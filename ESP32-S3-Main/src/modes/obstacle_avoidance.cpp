#include "obstacle_avoidance.h"
#include "../config/thresholds.h"

ObstacleAvoidance::ObstacleAvoidance(UltrasonicManager& us,
                                     MotionTracker& mpu,
                                     Display& lcd,
                                     Buzzer& buzz) {
    ultrasonic = &us;
    motion = &mpu;
    display = &lcd;
    buzzer = &buzz;
    state = STATE_CLEAR;
}

void ObstacleAvoidance::begin() {
    resetMovement();
}

void ObstacleAvoidance::resetMovement() {
    for (int i = 0; i < 4; i++) {
        movementCmd[i] = 0;
    }
}

void ObstacleAvoidance::update() {
    ultrasonic->update();
    motion->update();

    float frontDist = ultrasonic->getDistance(US_FRONT);
    float rearDist  = ultrasonic->getDistance(US_BACK);

    bool tilted = motion->isTilted(25.0); // threshold from flowchart intent

    resetMovement();

    /* -------- FLOWCHART LOGIC -------- */

    // MPU threshold exceeded
    if (tilted) {
        state = STATE_SLOW;
        movementCmd[0] = 1; // Forward allowed (slow handled by motor board)
    }
    // Front obstacle
    else if (frontDist < COLLISION_DISTANCE_FRONT) {
        state = STATE_STOP;
        buzzer->playTone(TONE_EMERGENCY);
    }
    // Rear obstacle
    else if (rearDist < COLLISION_DISTANCE_BACK) {
        state = STATE_NO_REVERSE;
        movementCmd[0] = 1; // Forward only
        buzzer->playTone(TONE_WARNING);
    }
    // Clear path
    else {
        state = STATE_CLEAR;
        movementCmd[0] = 1; // Forward
        movementCmd[1] = 1; // Backward
        movementCmd[2] = 1; // Left
        movementCmd[3] = 1; // Right
    }

    updateDisplay(frontDist, rearDist);
    sendMovementCommand();
}

void ObstacleAvoidance::sendMovementCommand() {
    // Format: <F,B,L,R>
    Serial.print("<");
    for (int i = 0; i < 4; i++) {
        Serial.print(movementCmd[i]);
        if (i < 3) Serial.print(",");
    }
    Serial.println(">");
}

void ObstacleAvoidance::updateDisplay(float front, float rear) {
    display->clear();

    display->setCursor(0, 0);
    display->print("Front:");
    display->print(front, 0);
    display->print("cm");

    display->setCursor(0, 1);
    display->print("Rear :");
    display->print(rear, 0);
    display->print("cm");

    display->setCursor(0, 2);
    display->print("State:");

    display->setCursor(0, 3);
    switch (state) {
        case STATE_CLEAR:
            display->print("CLEAR");
            break;
        case STATE_SLOW:
            display->print("SLOW");
            break;
        case STATE_STOP:
            display->print("STOP");
            break;
        case STATE_NO_REVERSE:
            display->print("DON'T REVERSE");
            break;
    }
}
