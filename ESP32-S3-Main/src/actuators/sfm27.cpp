#include "sfm27.h"
#include "../config/pins.h"
#include "config/constants.h"


Buzzer::Buzzer() {
    buzzer1Pin = BUZZER_1;
    buzzer2Pin = BUZZER_2;
    volume = 100; // default max
}

void Buzzer::begin() {
    pinMode(buzzer1Pin, OUTPUT);
    pinMode(buzzer2Pin, OUTPUT);
    
    buzzer1PatternActive = false;
    buzzer2PatternActive = false;
    lastBuzzer1Ring = false;
    lastBuzzer2Ring = false;
    
    stop();
}

void Buzzer::setVolume(int vol) {
    volume = constrain(vol, 0, 100);
}

void Buzzer::drive(uint16_t buzzerNum, uint16_t freq) {
    if (volume == 0) {
        stop();
        return;
    }
    
    // Drive pins independently
    if (buzzerNum == 1) {
        digitalWrite(buzzer1Pin, HIGH);
        Serial.println(" -> B1 HIGH");
    }
    else if (buzzerNum == 2) {
        digitalWrite(buzzer2Pin, HIGH);
        Serial.println(" -> B2 HIGH");
    }
}

void Buzzer::stop() {
    digitalWrite(buzzer1Pin, LOW);
    digitalWrite(buzzer2Pin, LOW);
}

void Buzzer::playTone(BuzzerTone tone) {
    startPattern(1, tone);
}

void Buzzer::startPattern(int buzzerNum, BuzzerTone tone) {
    int steps = 1;
    switch (tone) {
        case TONE_CONFIRM:   steps = 1; break;
        case TONE_WARNING:   steps = 3; break;
        case TONE_ERROR:     steps = 5; break;
        case TONE_EMERGENCY: steps = 19; break;
    }

    if (buzzerNum == 1) {
        Serial.println(" -> B1 Pattern Start");
        buzzer1PatternActive = true;
        buzzer1Step = 0;
        buzzer1TotalSteps = steps;
        buzzer1NextStepTime = millis();
    } else {
        Serial.println(" -> B2 Pattern Start");
        buzzer2PatternActive = true;
        buzzer2Step = 0;
        buzzer2TotalSteps = steps;
        buzzer2NextStepTime = millis();
    }
}

void Buzzer::update() {
    // Update Buzzer 1
    if (buzzer1PatternActive && millis() >= buzzer1NextStepTime) {
        if (buzzer1Step % 2 == 0) {
            drive(1, 0);
            buzzer1NextStepTime = millis() + 150;
        } else {
            digitalWrite(buzzer1Pin, LOW);
            buzzer1NextStepTime = millis() + 100;
        }
        buzzer1Step++;
        if (buzzer1Step >= buzzer1TotalSteps) {
            digitalWrite(buzzer1Pin, LOW);
            buzzer1PatternActive = false;
        }
    }

    // Update Buzzer 2
    if (buzzer2PatternActive && millis() >= buzzer2NextStepTime) {
        if (buzzer2Step % 2 == 0) {
            drive(2, 0);
            buzzer2NextStepTime = millis() + 150;
        } else {
            digitalWrite(buzzer2Pin, LOW);
            buzzer2NextStepTime = millis() + 100;
        }
        buzzer2Step++;
        if (buzzer2Step >= buzzer2TotalSteps) {
            digitalWrite(buzzer2Pin, LOW);
            buzzer2PatternActive = false;
        }
    }
}

void Buzzer::controlFromFirebase(bool buzzer01ring, bool buzzer02ring, int buzzersound) {
    if (buzzersound != lastVolume) {
        lastVolume = buzzersound;
        setVolume(buzzersound);
        Serial.printf("Buzzer volume: %d%%\n", buzzersound);
    }
    
    if (buzzer01ring && !lastBuzzer1Ring) {
        startPattern(1, TONE_CONFIRM);
    }
    lastBuzzer1Ring = buzzer01ring;
    
    if (buzzer02ring && !lastBuzzer2Ring) {
        startPattern(2, TONE_EMERGENCY);
    }
    lastBuzzer2Ring = buzzer02ring;
}

void Buzzer::singleBeep() {
    startPattern(1, TONE_CONFIRM);
}

void Buzzer::doubleBeep() {
    buzzer1PatternActive = true;
    buzzer1Step = 0;
    buzzer1TotalSteps = 3;
    buzzer1NextStepTime = millis();
}

void Buzzer::tripleBeep() {
    buzzer1PatternActive = true;
    buzzer1Step = 0;
    buzzer1TotalSteps = 5;
    buzzer1NextStepTime = millis();
}