#include <Arduino.h>
#include "SerialTransfer.h"

#define UART_TX 1
#define UART_RX 3

SerialTransfer myTransfer;

struct TestPacket {
    uint8_t command;
    uint8_t speed;
    uint32_t timestamp;
};

TestPacket txPacket;
TestPacket rxPacket;

void setup() {
    Serial.begin(115200, SERIAL_8N1, UART_RX, UART_TX);
    delay(2000);
    
    myTransfer.begin(Serial);
    
    Serial.println("\n========================================");
    Serial.println("   UART Communication Test");
    Serial.println("========================================");
    Serial.println("\nUART initialized on GPIO 1 (TX) and GPIO 3 (RX)");
    Serial.println("Send data from ESP32 S3 to test communication");
    Serial.println("This board will echo back any received data\n");
}

void loop() {
    // Check for received data
    if (myTransfer.available()) {
        myTransfer.rxObj(rxPacket);
        
        Serial.println("\n✓ Data received!");
        Serial.printf("  Command: %d\n", rxPacket.command);
        Serial.printf("  Speed: %d\n", rxPacket.speed);
        Serial.printf("  Timestamp: %lu\n", rxPacket.timestamp);
        
        // Echo back
        myTransfer.txObj(rxPacket);
        myTransfer.sendData(sizeof(rxPacket));
        
        Serial.println("  → Data echoed back\n");
    }
    
    // Send test packet every 2 seconds
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 2000) {
        lastSend = millis();
        
        txPacket.command = random(0, 8);
        txPacket.speed = random(0, 101);
        txPacket.timestamp = millis();
        
        myTransfer.txObj(txPacket);
        myTransfer.sendData(sizeof(txPacket));
        
        Serial.println("→ Test packet sent:");
        Serial.printf("  Command: %d\n", txPacket.command);
        Serial.printf("  Speed: %d\n", txPacket.speed);
        Serial.printf("  Timestamp: %lu\n\n", txPacket.timestamp);
    }
    
    delay(10);
}