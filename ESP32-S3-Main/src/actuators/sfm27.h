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
    bool buzzer1PatternActive;
    bool buzzer2PatternActive;
    int buzzer1Step;
    int buzzer2Step;
    unsigned long buzzer1NextStepTime;
    unsigned long buzzer2NextStepTime;
    int buzzer1TotalSteps;
    int buzzer2TotalSteps;
    int lastVolume;

    // Firebase state
    bool lastBuzzer1Ring;
    bool lastBuzzer2Ring;

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
    void startPattern(int buzzerNum, BuzzerTone tone);
};

#endif
