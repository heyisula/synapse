#include "ky040.h"
#include "../config/pins.h"

// Pointer to the instance for the ISR
static RotaryEncoder* isrInstance = nullptr;

// Interrupt Service Routine
void IRAM_ATTR encoderISR() {
    if (isrInstance) {
        isrInstance->handleISR();
    }
}

// Full Step Lookup Table
// This table is designed to handle full quadrature transitions.
// Index = (old_state << 2) | new_state
// We only care about pin states (2 bits).
// REVERSED DIRECTION: Swapped 1 and -1 from the original table.
const int8_t KNOWN_STATES[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};

RotaryEncoder::RotaryEncoder() {
    pinCLK = ROTARY_CLK;
    pinDT = ROTARY_DT;
    pinSW = ROTARY_SW;
    
    state = 0;
    encoderDelta = 0;
    
    buttonActiveLow = true; 
    buttonState = false;    
    buttonPressed = false;
    buttonReleased = false;
    longPressDetected = false;
    ignoreNextRelease = false;
    
    lastDebounceTime = 0;
    buttonPressTime = 0;
    
    isrInstance = this;
}

RotaryEncoder::~RotaryEncoder() {
    detachInterrupt(digitalPinToInterrupt(pinCLK));
    detachInterrupt(digitalPinToInterrupt(pinDT));
}

void RotaryEncoder::begin() {
    pinMode(pinCLK, INPUT_PULLUP);
    pinMode(pinDT, INPUT_PULLUP);
    pinMode(pinSW, INPUT_PULLUP);
    
    // Initial state
    uint8_t p1 = digitalRead(pinDT);
    uint8_t p2 = digitalRead(pinCLK);
    state = (p1 << 1) | p2;
    
    // Attach Interrupts to BOTH pins for maximum resolution
    attachInterrupt(digitalPinToInterrupt(pinCLK), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinDT), encoderISR, CHANGE);
}

void IRAM_ATTR RotaryEncoder::handleISR() {
    uint8_t p1 = digitalRead(pinDT);
    uint8_t p2 = digitalRead(pinCLK);
    
    uint8_t currentState = (p1 << 1) | p2;
    uint8_t transition = (state << 2) | currentState;
    
    // Using a lookup table is faster and cleaner for ISR
    encoderDelta += KNOWN_STATES[transition & 0x0F];
    
    state = currentState;
}

void RotaryEncoder::update() {
    // Button Logic (Non-blocking Debounce) stays in update()
    // because millis() inside ISR is frowned upon (though works on ESP32 usually)
    // and button events don't need microsecond latency.
    
    bool rawReading = (digitalRead(pinSW) == LOW); 
    
    if (rawReading != buttonState) {
        if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
            buttonState = rawReading;
            lastDebounceTime = millis();
            
            if (buttonState == true) {
                // PRESSED
                buttonPressed = true;
                buttonPressTime = millis();
                longPressDetected = false; // Reset flag
                
                // IMPORTANT: If we are already in a long-press state from *before*, 
                // we should have cleared it. This is a fresh press.
                ignoreNextRelease = false; 
                
            } else {
                // RELEASED
                // If we detected a long press during the hold, do NOT trigger release
                if (!ignoreNextRelease) {
                    buttonReleased = true;
                }
            }
        }
    } else {
        // State is stable. Check for Long Press.
        if (buttonState == true && !longPressDetected) {
            if (millis() - buttonPressTime > LONG_PRESS_DELAY) {
                longPressDetected = true;
                ignoreNextRelease = true; // SUPPRESS the future release event
            }
        }
    }
}

int RotaryEncoder::getDelta() {
    // Atomic read and reset
    noInterrupts();
    int d = encoderDelta;
    encoderDelta = 0;
    interrupts();
    
    if (d != 0) {
        // Standard KY-040 often creates 4 quadrature states per physical detent.
        // We use a static accumulator to handle fractional movements or missed checks.
        
        static int remainder = 0;
        int total = d + remainder;
        
        // Changed divisor from 2 to 4 to fix "double stepping" (skipping items)
        // If the encoder is 2-step per detent, change this to 2.
        // If the encoder is 4-step per detent (common), use 4.
        const int STEPS_PER_DETENT = 4;
        
        int steps = total / STEPS_PER_DETENT; 
        remainder = total % STEPS_PER_DETENT;
        
        return steps;
    }
    
    return 0;
}

bool RotaryEncoder::isButtonPressed() {
    if (buttonPressed) {
        buttonPressed = false;
        return true;
    }
    return false;
}

bool RotaryEncoder::isButtonReleased() {
    if (buttonReleased) {
        buttonReleased = false;
        return true;
    }
    return false;
}

bool RotaryEncoder::isLongPress() {
    if (longPressDetected) {
        longPressDetected = false; // Consume it
        return true;
    }
    return false;
}