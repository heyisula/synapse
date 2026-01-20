#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>

class RotaryEncoder {
private:
    uint8_t pinCLK;
    uint8_t pinDT;
    uint8_t pinSW;

    // Encoder state variables
    uint8_t lastEncoded;
    volatile int16_t encoderDelta; // Using volatile for potential interrupt safety/atomicity

    // Button state variables
    bool buttonState;        // Current stable state
    bool lastButtonState;    // Previous stable state
    bool buttonPressed;      // Flag for "just pressed" event (if needed)
    bool buttonReleased;     // Flag for "just released" event (which we want)
    bool longPressActive;
    
    unsigned long lastDebounceTime;
    unsigned long buttonPressTime;
    
    // Constants
    static const unsigned long DEBOUNCE_DELAY = 20; // Fast debounce
    static const unsigned long LONG_PRESS_DELAY = 1000;

public:
    RotaryEncoder();
    ~RotaryEncoder();
    
    void begin();
    void update();
    
    // Returns the change in position since last call (can be +ve or -ve)
    // Automatically resets delta to 0 after reading.
    int getDelta();
    
    // Button events
    bool isButtonPressed();    // Returns true once per press (on rising edge of press)
    bool isButtonReleased();   // Returns true once per release
    bool isLongPress();        // Returns true if held for long press duration
};

#endif