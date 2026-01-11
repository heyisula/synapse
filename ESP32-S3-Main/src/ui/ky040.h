#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>
#include <KY040.h>

class RotaryEncoder {
private:
    KY040* encoder;
    uint8_t swPin;
    int position;
    bool buttonPressed;
    unsigned long lastButtonTime;
    unsigned long buttonPressStart;
    bool lastButtonState;
    int lastSwState;
    unsigned long lastDebounceTime;
    static const unsigned long DEBOUNCE_DELAY = 50;
    static const unsigned long LONG_PRESS_DURATION = 1000;
    
public:
    RotaryEncoder();
    ~RotaryEncoder();
    void begin();
    void update();
    int getPosition();
    void resetPosition();
    bool isButtonPressed();
    bool isButtonReleased();
    bool isLongPress();
};

#endif