#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>

class RotaryEncoder {
private:
    uint8_t clkPin;
    uint8_t dtPin;
    uint8_t swPin;
    int position;
    int lastClkState;
    bool buttonPressed;
    unsigned long lastButtonTime;
    unsigned long buttonPressStart;
    
public:
    RotaryEncoder();
    void begin();
    void update();
    int getPosition();
    void resetPosition();
    bool isButtonPressed();
    bool isButtonReleased();
    bool isLongPress();
};

#endif