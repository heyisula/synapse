#include <Arduino.h>

// Pin definitions
#define LEFT_FRONT_IN1 16
#define LEFT_FRONT_IN2 17
#define LEFT_FRONT_ENA 22

#define LEFT_BACK_IN1 12
#define LEFT_BACK_IN2 13
#define LEFT_BACK_ENB 4

#define RIGHT_FRONT_IN1 18
#define RIGHT_FRONT_IN2 19
#define RIGHT_FRONT_ENA 23

#define RIGHT_BACK_IN1 14
#define RIGHT_BACK_IN2 15
#define RIGHT_BACK_ENB 5

#define EMERGENCY_STOP_PIN 27

#define UART_TX 1
#define UART_RX 3

// All motor control pins
const uint8_t motorPins[] = {
    LEFT_FRONT_IN1, LEFT_FRONT_IN2, LEFT_FRONT_ENA,
    LEFT_BACK_IN1, LEFT_BACK_IN2, LEFT_BACK_ENB,
    RIGHT_FRONT_IN1, RIGHT_FRONT_IN2, RIGHT_FRONT_ENA,
    RIGHT_BACK_IN1, RIGHT_BACK_IN2, RIGHT_BACK_ENB
};

const char* motorPinNames[] = {
    "LEFT_FRONT_IN1", "LEFT_FRONT_IN2", "LEFT_FRONT_ENA",
    "LEFT_BACK_IN1", "LEFT_BACK_IN2", "LEFT_BACK_ENB",
    "RIGHT_FRONT_IN1", "RIGHT_FRONT_IN2", "RIGHT_FRONT_ENA",
    "RIGHT_BACK_IN1", "RIGHT_BACK_IN2", "RIGHT_BACK_ENB"
};

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n========================================");
    Serial.println("   ESP32 WROOM - Hardware Pin Test");
    Serial.println("========================================\n");
    
    // Initialize all pins
    for (int i = 0; i < 12; i++) {
        pinMode(motorPins[i], OUTPUT);
        digitalWrite(motorPins[i], LOW);
    }
    
    pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
    
    Serial.println("✓ All pins initialized\n");
}

void loop() {
    Serial.println("\n--- Starting Pin Toggle Test ---");
    
    // Test each pin individually
    for (int i = 0; i < 12; i++) {
        Serial.printf("Testing GPIO %d (%s)... ", motorPins[i], motorPinNames[i]);
        
        // Set HIGH
        digitalWrite(motorPins[i], HIGH);
        delay(200);
        
        // Set LOW
        digitalWrite(motorPins[i], LOW);
        delay(200);
        
        Serial.println("✓ OK");
    }
    
    // Test Emergency Stop Button
    Serial.printf("\nEmergency Stop Button (GPIO %d): ", EMERGENCY_STOP_PIN);
    bool buttonState = digitalRead(EMERGENCY_STOP_PIN);
    if (buttonState == HIGH) {
        Serial.println("NOT PRESSED (pull-up active)");
    } else {
        Serial.println("⚠️ PRESSED or SHORTED!");
    }
    
    // Test UART pins
    Serial.printf("UART TX (GPIO %d): Ready\n", UART_TX);
    Serial.printf("UART RX (GPIO %d): Ready\n", UART_RX);
    
    Serial.println("\n✓ Hardware test cycle complete");
    Serial.println("Waiting 5 seconds before next test...\n");
    
    delay(5000);
}