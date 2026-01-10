#ifndef UART_H
#define UART_H

#include <Arduino.h>
#include "SerialTransfer.h"

enum MotorCommand : uint8_t {
    CMD_STOP = 0,
    CMD_FORWARD,
    CMD_BACKWARD,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_ROTATE_LEFT,
    CMD_ROTATE_RIGHT,
    CMD_EMERGENCY_STOP
};

class UARTProtocol {
private:
    HardwareSerial* serial;
    SerialTransfer transfer;
    
    MotorCommand lastReceivedCommand;
    uint8_t lastReceivedSpeed;
    bool newDataAvailable;

public:
    UARTProtocol();
    void begin();
    bool receiveMotorCommand(MotorCommand &cmd, uint8_t &speed);
    void sendAcknowledgment(MotorCommand cmd, uint8_t speed);
    bool isNewDataAvailable();
    void clearNewDataFlag();
};

#endif