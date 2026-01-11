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
    stop();
}

void Buzzer::setVolume(int vol) {
    volume = constrain(vol, 0, 100);
}

void Buzzer::drive(uint16_t cycles, uint16_t freq) {
    uint32_t halfPeriod = 500000 / freq; // microseconds
    uint16_t effectiveCycles = map(volume, 0, 100, 0, cycles);

    for (uint16_t i = 0; i < effectiveCycles; i++) {
        digitalWrite(buzzer1Pin, HIGH);
        digitalWrite(buzzer2Pin, LOW);
        delayMicroseconds(halfPeriod);

        digitalWrite(buzzer1Pin, LOW);
        digitalWrite(buzzer2Pin, HIGH);
        delayMicroseconds(halfPeriod);
    }

    // Leave pins LOW after beep
    digitalWrite(buzzer1Pin, LOW);
    digitalWrite(buzzer2Pin, LOW);
}

void Buzzer::playTone(BuzzerTone tone) {
    switch (tone) {
        case TONE_CONFIRM:   singleBeep(); break;
        case TONE_WARNING:   doubleBeep(); break;
        case TONE_ERROR:     tripleBeep(); break;
        case TONE_EMERGENCY: emergencyAlarm(); break;
    }
}

void Buzzer::singleBeep() {
    drive(400, CONFIRM_FREQ); // ~120ms
}

void Buzzer::doubleBeep() {
    drive(300, WARNING_FREQ);
    delay(80);
    drive(300, WARNING_FREQ);
}

void Buzzer::tripleBeep() {
    drive(250, ERROR_FREQ);
    delay(70);
    drive(250, ERROR_FREQ);
    delay(70);
    drive(250, ERROR_FREQ);
}

void Buzzer::emergencyAlarm() {
    for (int i = 0; i < 10; i++) {
        drive(800, EMERGENCY_FREQ);
        delay(200);
    }
}

void Buzzer::stop() {
    digitalWrite(buzzer1Pin, LOW);
    digitalWrite(buzzer2Pin, LOW);
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
            // Buzzer 1 turned ON
            buzzer1Active = true;
            Serial.println("┌─────────────────────────");
            Serial.println("│ BUZZER 1: ON");
            Serial.print("│ Volume: ");
            Serial.print(volume);
            Serial.println("%");
            Serial.println("└─────────────────────────");
        }
        singleBeep();
        
    } else {
        if (buzzer1Active) {
            // Buzzer 1 turned OFF
            buzzer1Active = false;
            Serial.println("┌─────────────────────────");
            Serial.println("│ BUZZER 1: OFF");
            Serial.println("└─────────────────────────");
        }
    }
    
    if (buzzer02ring) {
        if (!buzzer2Active) {
            // Buzzer 2 turned ON
            buzzer2Active = true;
            Serial.println("┌─────────────────────────");
            Serial.println("│ BUZZER 2: ON");
            Serial.print("│ Volume: ");
            Serial.print(volume);
            Serial.println("%");
            Serial.println("└─────────────────────────");
        }
        
        emergencyAlarm();
        
    } else {
        if (buzzer2Active) {
            // Buzzer 2 turned OFF
            buzzer2Active = false;
            Serial.println("┌─────────────────────────");
            Serial.println("│ BUZZER 2: OFF");
            Serial.println("└─────────────────────────");
        }
    }
    
    // If both buzzers are OFF, stop all output
    if (!buzzer01ring && !buzzer02ring) {
        stop();
    }
}