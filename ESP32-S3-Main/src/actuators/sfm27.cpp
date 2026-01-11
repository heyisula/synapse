#include "sfm27.h"
#include "../config/pins.h"
#include "config/constants.h"

// Adjustable for best loudness
#define CONFIRM_FREQ    1800
#define WARNING_FREQ    2600
#define ERROR_FREQ      1400
#define EMERGENCY_FREQ  3000

Buzzer::Buzzer() {
    buzzer1Pin = BUZZER_1;
    buzzer2Pin = BUZZER_2;
    volume = 100; // default max
}

void Buzzer::begin() {
    pinMode(buzzer1Pin, OUTPUT);
    pinMode(buzzer2Pin, OUTPUT);
    
    patternActive = false;
    currentStep = 0;
    nextStepTime = 0;
    
    stop();
}

void Buzzer::setVolume(int vol) {
    volume = constrain(vol, 0, 100);
}

// For active buzzers, we ignore frequency and use steady push-pull
void Buzzer::drive(uint16_t duration_ms, uint16_t freq) {
    if (volume == 0) {
        stop();
        return;
    }
    
    // Max loudness: Push-pull (one pin HIGH, one pin LOW)
    digitalWrite(buzzer1Pin, HIGH);
    digitalWrite(buzzer2Pin, LOW);
}

void Buzzer::stop() {
    digitalWrite(buzzer1Pin, LOW);
    digitalWrite(buzzer2Pin, LOW);
}

void Buzzer::playTone(BuzzerTone tone) {
    patternActive = true;
    patternStartTime = millis();
    currentStep = 0;
    
    switch (tone) {
        case TONE_CONFIRM:   totalSteps = 1; break; // 1 beep
        case TONE_WARNING:   totalSteps = 3; break; // Beep, Sil, Beep
        case TONE_ERROR:     totalSteps = 5; break; // Beep, Sil, Beep, Sil, Beep
        case TONE_EMERGENCY: totalSteps = 19; break; // Rapid pulses
    }
    nextStepTime = millis();
}

void Buzzer::update() {
    if (!patternActive) return;

    if (millis() >= nextStepTime) {
        // Step logic: Even steps are ON, Odd steps are OFF
        if (currentStep % 2 == 0) {
            drive(0, 0); // Sound ON
            nextStepTime = millis() + 150; // Duration 150ms
        } else {
            stop(); // Sound OFF
            nextStepTime = millis() + 100; // Gap 100ms
        }

        currentStep++;
        if (currentStep >= totalSteps) {
            stop();
            patternActive = false;
        }
    }
}

void Buzzer::controlFromFirebase(bool buzzer01ring, bool buzzer02ring, int buzzersound) {
    if (buzzersound != lastVolume) {
        lastVolume = buzzersound;
        setVolume(buzzersound);
        
        Serial.print("Buzzer volume set to: ");
        Serial.print(buzzersound);
        Serial.println("%");
    }
    
    if (buzzer01ring) {
        if (!buzzer1Active) {
            buzzer1Active = true;
            playTone(TONE_CONFIRM);
        }
    } else {
        buzzer1Active = false;
    }
    
    if (buzzer02ring) {
        if (!buzzer2Active) {
            buzzer2Active = true;
            playTone(TONE_EMERGENCY);
        }
    } else {
        buzzer2Active = false;
    }
}