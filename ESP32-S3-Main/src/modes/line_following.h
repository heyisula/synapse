#ifndef LINE_FOLLOWING_H
#define LINE_FOLLOWING_H

#include <Arduino.h>
#include "sensors/bfd1000.h"
#include "communication/uart.h"
#include "actuators/ermc1604syg.h"
#include "config/constants.h"

class LineFollowing {
private:
    BFD1000* sensor;
    UARTProtocol* uart;
    Display* display;

    bool isActive;
    int baseSpeed;
    float lastError;
    
    // PID-like constants for steering
    const float KP = 20.0f;
    const float KD = 10.0f;

    void updateSteering();
    float calculateError();

public:
    LineFollowing(BFD1000* s, UARTProtocol* u, Display* d);
    
    void begin();
    void start();
    void stop();
    void update();
    
    bool isLineDetected();
};

#endif
