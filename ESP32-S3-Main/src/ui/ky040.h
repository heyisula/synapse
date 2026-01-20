#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>

class RotaryEncoder {
private:
    uint8_t pinCLK;
    uint8_t pinDT;
    // Encoder state variables
    volatile uint8_t state;           
    volatile int encoderDelta; 

    // Button state variables
    uint8_t pinSW;
    bool buttonActiveLow;
    bool buttonState;        
    bool buttonPressed;      
    bool buttonReleased;     
    bool longPressDetected;  
    bool ignoreNextRelease;  
    
    unsigned long lastDebounceTime;
    unsigned long buttonPressTime;
    
    // Constants
    static const unsigned long DEBOUNCE_DELAY = 50; 
    static const unsigned long LONG_PRESS_DELAY = 800; // Faster long press (0.8s)

public:
    RotaryEncoder();
    ~RotaryEncoder();
    
    void begin();
    void update(); // Still needed for button polling
    
    // ISR for Encoder Rotation
    void IRAM_ATTR handleISR();

    int getDelta();
    
    bool isButtonPressed();    
    bool isButtonReleased();   
    bool isLongPress();        

};

#endif