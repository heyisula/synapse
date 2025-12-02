#include "state_machine.h"

StateMachine::StateMachine() {
    currentState = STATE_IDLE;
    previousState = STATE_IDLE;
    stateStartTime = 0;
}


void StateMachine::begin() {
    currentState = STATE_IDLE;
    stateStartTime = millis();
}

void StateMachine::setState(SystemState newState) {
    if(newState != currentState) {
        previousState = currentState;
        currentState = newState;
        stateStartTime = millis();
    }
}

SystemState StateMachine::getState() {
    return currentState;
}

SystemState StateMachine::getPreviousState() {
    return previousState;
}

unsigned long StateMachine::getStateTime() {
    return millis() - stateStartTime;
}

bool StateMachine::isState(SystemState state) {
    return currentState == state;
}