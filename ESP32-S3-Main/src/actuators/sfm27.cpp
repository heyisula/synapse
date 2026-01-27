#include "sfm27.h"
#include "../config/pins.h"
#include "config/constants.h"


Buzzer::Buzzer() {
    buzzer1Pin = BUZZER_1;
    buzzer2Pin = BUZZER_2;
    volume = 100; // default max
    patternActive = false;
    currentStep = 0;
    taskHandle = NULL;
}

void Buzzer::taskWorker(void* _this) {
    Buzzer* buzzer = (Buzzer*)_this;
    while (true) {
        buzzer->process();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void Buzzer::begin() {
    pinMode(buzzer1Pin, OUTPUT);
    pinMode(buzzer2Pin, OUTPUT);
    
    activeChannels = BUZZER_CH_BOTH;
    
    stop();

    // FreeRTOS task for non-blocking execution
    if (taskHandle == NULL) {
        xTaskCreatePinnedToCore(
            taskWorker,   // Function
            "BuzzerTask", // Name
            2048,         // Stack size
            this,         // Param
            1,            // Priority
            &taskHandle,  // Handle
            1             // Core (Application Core)
        );
    }
}

void Buzzer::setVolume(int vol) {
    volume = constrain(vol, 0, 100);
}

void Buzzer::drive(uint16_t duration_ms, uint16_t freq) {
    if (volume == 0) {
        digitalWrite(buzzer1Pin, LOW);
        digitalWrite(buzzer2Pin, LOW);
        return;
    }
    
    if (activeChannels & BUZZER_CH_1) {
        digitalWrite(buzzer1Pin, HIGH);
    } else {
        digitalWrite(buzzer1Pin, LOW);
    }
    
    if (activeChannels & BUZZER_CH_2) {
        digitalWrite(buzzer2Pin, HIGH);
    } else {
        digitalWrite(buzzer2Pin, LOW);
    }
}

void Buzzer::stop() {
    patternActive = false;
    
    digitalWrite(buzzer1Pin, LOW);
    digitalWrite(buzzer2Pin, LOW);
}

void Buzzer::playTone(BuzzerTone tone, uint8_t channels) {
    // Stop previous pattern
    patternActive = false; 
    delay(5); // Small delay to ensuring clean state
    
    currentStep = 0;
    activeChannels = channels;
    
    switch (tone) {
        case TONE_CONFIRM:   totalSteps = 1; break; // 1 beep
        case TONE_WARNING:   totalSteps = 3; break; // Beep, Sil, Beep
        case TONE_ERROR:     totalSteps = 5; break; // Beep, Sil, Beep, Sil, Beep
        case TONE_EMERGENCY: totalSteps = 19; break; // Rapid pulses
    }
    
    patternStartTime = millis();
    nextStepTime = millis();
    patternActive = true;
}


void Buzzer::process() {
    if (!patternActive) return;

    if (millis() >= nextStepTime) {
        // Check if we have completed all steps
        if (currentStep >= totalSteps) {
            stop();
            return;
        }

        // Step logic: Even steps are ON, Odd steps are OFF
        if (currentStep % 2 == 0) {
            drive(0, 0); // Sound ON
            nextStepTime = millis() + 400; // Duration 400ms
        } else {
            // Turn off sound but keep pattern active
            digitalWrite(buzzer1Pin, LOW);
            digitalWrite(buzzer2Pin, LOW);
            
            nextStepTime = millis() + 200; // Gap 200ms
        }

        currentStep++;
    }
}


// Kept empty to prevent breaking existing code calls.
void Buzzer::update() {
    // Logic moved to process() called by taskWorker
}

void Buzzer::controlFromFirebase(bool buzzer01ring, bool buzzer02ring, int buzzersound) {
    if (buzzersound != lastVolume) {
        lastVolume = buzzersound;
        setVolume(buzzersound);
        
        Serial.print("Buzzer volume set to: ");
        Serial.print(buzzersound);
        Serial.println("%");
    }
    
    // Handle Buzzer 1 Request (Warning Tone)
    if (buzzer01ring) {
        if (!buzzer1Active) {
            buzzer1Active = true;
            playTone(TONE_WARNING, BUZZER_CH_1);
        }
    } else {
        if (buzzer1Active) {
            buzzer1Active = false;
            // Only stop if this was the active channel
            if (patternActive && (activeChannels == BUZZER_CH_1)) {
                stop();
            }
        }
    }
    
    // Handle Buzzer 2 Request (Emergency Tone)
    if (buzzer02ring) {
        if (!buzzer2Active) {
            buzzer2Active = true;
            playTone(TONE_EMERGENCY, BUZZER_CH_2);
        }
    } else {
        if (buzzer2Active) {
            buzzer2Active = false;
            if (patternActive && (activeChannels == BUZZER_CH_2)) {
                stop();
            }
        }
    }
}

void Buzzer::singleBeep() {
    playTone(TONE_CONFIRM);
}

void Buzzer::doubleBeep() {
    patternActive = false;
    currentStep = 0;
    activeChannels = BUZZER_CH_BOTH;
    totalSteps = 3; 
    nextStepTime = millis();
    patternStartTime = millis();
    patternActive = true;
}

void Buzzer::tripleBeep() {
    patternActive = false;
    currentStep = 0;
    activeChannels = BUZZER_CH_BOTH;
    totalSteps = 5;
    nextStepTime = millis();
    patternStartTime = millis();
    patternActive = true;
}