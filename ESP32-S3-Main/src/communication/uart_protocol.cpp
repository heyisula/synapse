#include "uart_protocol.h"
#include "../config/pins.h"
#include "../config/constants.h"

UARTProtocol::UARTProtocol() {
    serial = &Serial1;
    lastSendTime = 0;
}

void UARTProtocol::begin() {
    serial->begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX, UART_TX);
}

uint8_t UARTProtocol::calculateChecksum(MotorControlPacket* packet) {
    return packet->command ^ packet->speed;
}

void UARTProtocol::sendMotorCommand(MotorCommand cmd, uint8_t speed) {
    MotorControlPacket packet;
    packet.command = cmd;
    packet.speed = constrain(speed, 0, 100);
    packet.checksum = calculateChecksum(&packet);
    
    serial->write((uint8_t*)&packet, sizeof(packet));
    lastSendTime = millis();
}

void UARTProtocol::sendEmergencyStop() {
    sendMotorCommand(CMD_EMERGENCY_STOP, 0);
}

bool UARTProtocol::receiveAcknowledgment() {
    if(serial->available() >= 1) {
        uint8_t ack = serial->read();
        return ack == 0xAA;
    }
    return false;
}