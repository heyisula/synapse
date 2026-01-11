#ifndef OBSTACLE_AVOIDANCE_H
#define OBSTACLE_AVOIDANCE_H

#include <Arduino.h>
#include "../sensors/hcsr04.h"
#include "../sensors/mpu6050.h"
#include "../actuators/ermc1604syg.h"
#include "../actuators/sfm27.h"

enum SafetyState {
    STATE_CLEAR,
    STATE_SLOW,
    STATE_STOP,
    STATE_NO_REVERSE
};

class ObstacleAvoidance {
private:
    UltrasonicManager* ultrasonic;
    MotionTracker* motion;
    Display* display;
    Buzzer* buzzer;

    SafetyState state;

    int movementCmd[4]; // Forward, Backward, Left, Right

    void resetMovement();
    void sendMovementCommand();
    void updateDisplay(float front, float rear);

public:
    ObstacleAvoidance(UltrasonicManager& us,
                      MotionTracker& mpu,
                      Display& lcd,
                      Buzzer& buzz);

    void begin();
    void update();   // Call inside loop()
};

#endif
