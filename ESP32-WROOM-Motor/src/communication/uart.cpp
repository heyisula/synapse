#include "uart.h"
#include "../config/pins.h"

#define UART_BAUD_RATE 115200

UARTProtocol::UARTProtocol() {
    serial = &Serial;  // Using Serial (UART0) on ESP32 WROOM
    newDataAvailable = false;
    lastReceivedCommand = CMD_STOP;
    lastReceivedSpeed = 0;
}

void UARTProtocol::begin() {
    serial->begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX, UART_TX);
    transfer.begin(*serial);
    Serial.println("UART Communication Started - ESP32 WROOM");
}

bool UARTProtocol::receiveMotorCommand(MotorCommand &cmd, uint8_t &speed) {
    // Check if a full packet has been received
    if (transfer.available()) {
        // Read command and speed from receive buffer
        transfer.rxObj(cmd, 0);
        transfer.rxObj(speed, 1);
        
        lastReceivedCommand = cmd;
        lastReceivedSpeed = speed;
        newDataAvailable = true;
        
        // Send acknowledgment back to ESP32 S3
        sendAcknowledgment(cmd, speed);
        
        return true;
    }
    return false;
}

void UARTProtocol::sendAcknowledgment(MotorCommand cmd, uint8_t speed) {
    // Echo back the received command and speed as acknowledgment
    transfer.txObj(cmd, 0);
    transfer.txObj(speed, 1);
    transfer.sendData(2);
}

bool UARTProtocol::isNewDataAvailable() {
    return newDataAvailable;
}

void UARTProtocol::clearNewDataFlag() {
    newDataAvailable = false;
}