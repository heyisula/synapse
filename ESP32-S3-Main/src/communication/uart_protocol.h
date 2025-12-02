#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#include <Arduino.h>

enum MotorCommand {
    CMD_STOP = 0,
    CMD_FORWARD,
    CMD_BACKWARD,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_ROTATE_LEFT,
    CMD_ROTATE_RIGHT,
    CMD_EMERGENCY_STOP
};

struct MotorControlPacket {
    uint8_t command;
    uint8_t speed;
    uint8_t checksum;
};

class UARTProtocol {
private:
    HardwareSerial* serial;
    unsigned long lastSendTime;
    
    uint8_t calculateChecksum(MotorControlPacket* packet);
    
public:
    UARTProtocol();
    void begin();
    void sendMotorCommand(MotorCommand cmd, uint8_t speed);
    void sendEmergencyStop();
    bool receiveAcknowledgment();
};

#endif