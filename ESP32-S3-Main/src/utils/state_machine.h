#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>

enum SystemState {
    STATE_IDLE,
    STATE_FOLLOWING,
    STATE_NAVIGATION,
    STATE_MONITORING,
    STATE_OBSTACLE_AVOIDANCE,
    STATE_ENVIRONMENTAL_CHECK,
    STATE_EMERGENCY,
    STATE_ERROR
};

class StateMachine {
private:
    SystemState currentState;
    SystemState previousState;
    unsigned long stateStartTime;
    
public:
    StateMachine();
    void begin();
    void setState(SystemState newState);
    SystemState getState();
    SystemState getPreviousState();
    unsigned long getStateTime();
    bool isState(SystemState state);
};

#endif