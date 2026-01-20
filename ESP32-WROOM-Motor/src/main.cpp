#include <Arduino.h>
#define EMERGENCY_PIN 27

bool emergencyLatched = false;
bool lastState = HIGH;

void setup() {
    Serial.begin(115200);
    pinMode(EMERGENCY_PIN, INPUT_PULLUP);
}

void loop() {
    bool currentState = digitalRead(EMERGENCY_PIN);

    // Detect button press (HIGH → LOW)
    if (lastState == HIGH && currentState == LOW) {
        emergencyLatched = true;
        Serial.println("EMERGENCY STOP ACTIVATED");
    }

    lastState = currentState;

    if (emergencyLatched) {
        // STOP EVERYTHING HERE
        Serial.println("SYSTEM HALTED");
        delay(500);
        return;
    }

    // Normal operation here
}
