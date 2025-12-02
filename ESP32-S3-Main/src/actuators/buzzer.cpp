#include "buzzer.h"
#include "../config/pins.h"

Buzzer::Buzzer() {
    buzzer1Pin = BUZZER_1;
    buzzer2Pin = BUZZER_2;
    volume = 5;
}

void Buzzer::begin() {
    pinMode(buzzer1Pin, OUTPUT);
    pinMode(buzzer2Pin, OUTPUT);
    digitalWrite(buzzer1Pin, LOW);
    digitalWrite(buzzer2Pin, LOW);
}

void Buzzer::setVolume(int vol) {
    volume = constrain(vol, 0, 10);
}

void Buzzer::playTone(BuzzerTone tone) {
    switch(tone) {
        case TONE_CONFIRM:
            singleBeep();
            break;
        case TONE_WARNING:
            doubleBeep();
            break;
        case TONE_ERROR:
            tripleBeep();
            break;
        case TONE_EMERGENCY:
            emergencyAlarm();
            break;
        case TONE_HEARTBEAT:
            singleBeep();
            break;
    }
}

void Buzzer::singleBeep() {
    digitalWrite(buzzer1Pin, HIGH);
    delay(100);
    digitalWrite(buzzer1Pin, LOW);
}

void Buzzer::doubleBeep() {
    singleBeep();
    delay(100);
    singleBeep();
}

void Buzzer::tripleBeep() {
    singleBeep();
    delay(100);
    singleBeep();
    delay(100);
   singleBeep();
}

void Buzzer::emergencyAlarm() {
    for(int i = 0; i < 5; i++) {
        digitalWrite(buzzer1Pin, HIGH);
        digitalWrite(buzzer2Pin, HIGH);
        delay(200);
        digitalWrite(buzzer1Pin, LOW);
        digitalWrite(buzzer2Pin, LOW);
        delay(200);
    }
}

void Buzzer::stop() {
    digitalWrite(buzzer1Pin, LOW);
    digitalWrite(buzzer2Pin, LOW);
}