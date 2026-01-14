#ifndef UART_H
#define UART_H

#include <Arduino.h>
#include "SerialTransfer.h"

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


class UARTProtocol {
private:
    HardwareSerial* serial;
    SerialTransfer transfer;

    unsigned long lastSendTime;
    

public:
    UARTProtocol();
    void begin();
    void sendMotorCommand(MotorCommand cmd, uint8_t speed);
    void sendEmergencyStop();
    bool receiveAcknowledgment(MotorCommand &cmd, uint8_t &speed);
};

#endif