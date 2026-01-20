#ifndef SFM27_H
#define SFM27_H

#include <Arduino.h>

enum BuzzerTone {
    TONE_CONFIRM,
    TONE_WARNING,
    TONE_ERROR,
    TONE_EMERGENCY
};

class Buzzer {
private:
    uint8_t buzzer1Pin;
    uint8_t buzzer2Pin;
    uint8_t volume; // 0-100

    void drive(uint16_t cycles, uint16_t freq);

    // Buzzer state tracking
    bool buzzer1Active;
    bool buzzer2Active;
    int lastVolume;

public:
    Buzzer();
    void begin();
    void setVolume(int vol);
    void playTone(BuzzerTone tone);

    void singleBeep();
    void doubleBeep();
    void tripleBeep();
    void emergencyAlarm();
    void stop();
    void update(); // Must be called in main loop

    void controlFromFirebase(bool buzzer01ring, bool buzzer02ring, int buzzersound);

private:
    unsigned long patternStartTime;
    unsigned long nextStepTime;
    int currentStep;
    int totalSteps;
    bool patternActive;
    
    // Pattern definitions
    void startPattern(int type);
};

#endif
