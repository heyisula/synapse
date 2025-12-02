#ifndef OBSTACLE_MODE_H
#define OBSTACLE_MODE_H

#include <Arduino.h>
#include "../sensors/ultrasonic.h"
#include "../communication/uart_protocol.h"
#include "../actuators/buzzer.h"

enum ObstacleState {
    OBS_CLEAR,
    OBS_DETECTED,
    OBS_EMERGENCY
};

class ObstacleMode {
private:
    Ultrasonic* ultrasonic;
    UARTProtocol* uart;
    Buzzer* buzzer;
    ObstacleState state;
    bool obstacleCleared;
    
public:
    ObstacleMode(Ultrasonic* us, UARTProtocol* u, Buzzer* bz);
    void begin();
    void update();
    void handleObstacle();
    void avoidObstacle();  // ✓ FIXED: Removed typo "ObsZtacle"
    bool isPathClear();
};

#endif