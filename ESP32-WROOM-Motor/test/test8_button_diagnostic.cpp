#include <Arduino.h>

/*
 * EMERGENCY BUTTON DIAGNOSTIC TOOL
 * 
 * This test monitors the emergency button in real-time to diagnose
 * mechanical issues like bouncing, multiple contacts, or inconsistent behavior.
 * 
 * What it shows:
 * - Raw button state (HIGH/LOW) every 10ms
 * - State changes with timestamps
 * - Bounce detection
 * - Press/release duration
 * 
 * How to use:
 * 1. Upload this test
 * 2. Open serial monitor
 * 3. Press the button slowly, halfway, fully
 * 4. Watch for unexpected state changes
 */

#define EMERGENCY_STOP_PIN 27

// State tracking
bool lastState = HIGH;
unsigned long lastChangeTime = 0;
unsigned long pressStartTime = 0;
unsigned long releaseStartTime = 0;
int changeCount = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n========================================");
    Serial.println("   EMERGENCY BUTTON DIAGNOSTIC TOOL");
    Serial.println("========================================\n");
    
    pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
    
    Serial.println("Configuration:");
    Serial.println("  Pin: GPIO 27");
    Serial.println("  Mode: INPUT_PULLUP");
    Serial.println("  Active: LOW (button pressed)");
    Serial.println("  Idle: HIGH (button released)\n");
    
    Serial.println("Monitoring started...");
    Serial.println("Press the button and watch for state changes:\n");
    
    lastState = digitalRead(EMERGENCY_STOP_PIN);
    Serial.print("Initial state: ");
    Serial.println(lastState == HIGH ? "HIGH (Released)" : "LOW (Pressed)");
    Serial.println("----------------------------------------\n");
}

void loop() {
    bool currentState = digitalRead(EMERGENCY_STOP_PIN);
    unsigned long now = millis();
    
    // Detect state change
    if (currentState != lastState) {
        unsigned long timeSinceLastChange = now - lastChangeTime;
        changeCount++;
        
        // Print timestamp and state change
        Serial.print("[");
        Serial.print(now);
        Serial.print(" ms] ");
        
        if (currentState == LOW) {
            // Button pressed
            Serial.print("🔴 PRESSED (HIGH → LOW)");
            pressStartTime = now;
            
            if (timeSinceLastChange < 50) {
                Serial.print(" ⚠️ BOUNCE! (");
                Serial.print(timeSinceLastChange);
                Serial.print(" ms since last change)");
            }
            
            if (releaseStartTime > 0) {
                unsigned long releaseDuration = now - releaseStartTime;
                Serial.print(" | Released for: ");
                Serial.print(releaseDuration);
                Serial.print(" ms");
            }
            
        } else {
            // Button released
            Serial.print("🟢 RELEASED (LOW → HIGH)");
            releaseStartTime = now;
            
            if (timeSinceLastChange < 50) {
                Serial.print(" ⚠️ BOUNCE! (");
                Serial.print(timeSinceLastChange);
                Serial.print(" ms since last change)");
            }
            
            if (pressStartTime > 0) {
                unsigned long pressDuration = now - pressStartTime;
                Serial.print(" | Pressed for: ");
                Serial.print(pressDuration);
                Serial.print(" ms");
            }
        }
        
        Serial.print(" | Change #");
        Serial.println(changeCount);
        
        lastState = currentState;
        lastChangeTime = now;
    }
    
    delay(1);  // Fast polling for accurate detection
}
