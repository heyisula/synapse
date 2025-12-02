#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

enum BuzzerTone {
    TONE_CONFIRM,
    TONE_WARNING,
    TONE_ERROR,
    TONE_EMERGENCY,
    TONE_HEARTBEAT
};

class Buzzer {
private:
    uint8_t buzzer1Pin;
    uint8_t buzzer2Pin;
    int volume;
    
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
};

#endif