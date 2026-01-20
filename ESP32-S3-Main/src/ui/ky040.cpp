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
const int8_t KNOWN_STATES[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

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
    
    // Optional: Divisor if it's too sensitive (2 ticks per step)
    // The user said "not changing for EACH tick", implying it was too slow/insensitive.
    // So we return raw delta. However, KY-040 often does 2 transitions per click.
    // If d is 2, and we want 1 step.
    
    // Logic: If user complains of "no responsiveness", raw is safer. 
    // They can filter in menu if it's too fast.
    // But usually, 1 physical click = 2 or 4 ISR steps.
    // Let's return d / 2 for standard KY-040 feel, BUT
    // the user insists on responsiveness.
    // Let's try 1:1 first. If it double-jumps, that's better than no-jumping.
    // Actually, let's implement the classic "divide by 2" signal because 
    // real detents usually span a full quadrature cycle (4) or half (2).
    
    // REVISION: User said "not responding to EACH tick".
    // This implies they turn it, click, and nothing happens.
    // This means we were MISSING counts.
    // So we should return the raw accumulation, or maybe / 2 if we are sure.
    // Let's return d / 2 but only if d >= 2.
    
    if (d != 0) {
        // Standard KY-040 has 2 signal changes per detent.
        // So raw 'd' will be 2 or -2 per click.
        // If we return 2, the menu jumps 2 items.
        // We probably want to return d / 2.
        
        // Simple software divisor
        // return d / 2; 
        
        // But what if we only got 1? (half turn).
        // If we integer divide 1/2 = 0, we lose it.
        // Let's use a static accumulator for the fractional part?
        // No, let's just return d and let the menu handle it? 
        // No, menu expects +1/-1 usually.
        
        // Let's go with: Only trigger on even counts.
        static int remainder = 0;
        int total = d + remainder;
        
        int steps = total / 2; 
        remainder = total % 2;
        
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