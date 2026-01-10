#include <Arduino.h>

#define EMERGENCY_STOP_PIN 27
#define LED_PIN 2  // Built-in LED

volatile bool emergencyTriggered = false;
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 50;

void IRAM_ATTR emergencyStopISR() {
    unsigned long currentTime = millis();
    if (currentTime - lastInterruptTime > debounceDelay) {
        emergencyTriggered = true;
        lastInterruptTime = currentTime;
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    
    attachInterrupt(digitalPinToInterrupt(EMERGENCY_STOP_PIN), 
                    emergencyStopISR, FALLING);
    
    Serial.println("\n========================================");
    Serial.println("   Emergency Stop Test");
    Serial.println("========================================\n");
    Serial.println("Emergency button configured on GPIO 27");
    Serial.println("Press the button to test...\n");
    
    digitalWrite(LED_PIN, LOW);
}

void loop() {
    // Check button state
    bool buttonPressed = (digitalRead(EMERGENCY_STOP_PIN) == LOW);
    
    // Display status
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        
        Serial.print("Button Status: ");
        if (buttonPressed) {
            Serial.println("🔴 PRESSED");
        } else {
            Serial.println("🟢 Released");
        }
    }
    
    // Handle interrupt trigger
    if (emergencyTriggered) {
        emergencyTriggered = false;
        
        Serial.println("\n⚠️⚠️⚠️ EMERGENCY STOP ACTIVATED! ⚠️⚠️⚠️");
        Serial.printf("Interrupt triggered at: %lu ms\n", lastInterruptTime);
        Serial.println("All motors would be stopped now!\n");
        
        // Blink LED rapidly
        for (int i = 0; i < 10; i++) {
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
        }
    }
    
    // Visual indicator
    if (buttonPressed) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }
    
    delay(10);
}