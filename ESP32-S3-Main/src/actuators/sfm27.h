#ifndef SFM27_H
#define SFM27_H

#include <Arduino.h>

enum BuzzerTone {
    TONE_CONFIRM,
    TONE_WARNING,
    TONE_ERROR,
    TONE_EMERGENCY
};

enum BuzzerChannel {
    BUZZER_CH_1 = 1,
    BUZZER_CH_2 = 2,
    BUZZER_CH_BOTH = 3
};

class Buzzer {
private:
    uint8_t buzzer1Pin;
    uint8_t buzzer2Pin;
    uint8_t volume; // 0-100
    uint8_t activeChannels;

    void drive(uint16_t cycles, uint16_t freq);

    // Buzzer state tracking
    bool buzzer1Active;
    bool buzzer2Active;
    int lastVolume;

public:
    Buzzer();
    void begin();
    void setVolume(int vol);
    void playTone(BuzzerTone tone, uint8_t channels = BUZZER_CH_BOTH);

    void singleBeep();
    void doubleBeep();
    void tripleBeep();
    void emergencyAlarm();
    void stop();
    void update(); 

    void controlFromFirebase(bool buzzer01ring, bool buzzer02ring, int buzzersound);

private:
    unsigned long patternStartTime;
    unsigned long nextStepTime;
    int currentStep;
    int totalSteps;
    bool patternActive;
    void startPattern(int type);
    
    TaskHandle_t taskHandle;
    static void taskWorker(void* _this);
    void process();
};

#endif
