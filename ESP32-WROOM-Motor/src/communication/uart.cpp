#include "uart.h"
#include "config/pins.h"
#include "config/constants.h"
#include "config/debug.h"


UARTProtocol::UARTProtocol() {
    serial = &Serial;
    newDataAvailable = false;
    lastReceivedCommand = CMD_STOP;
    lastReceivedSpeed = 0;
}

void UARTProtocol::begin() {
    serial->begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX, UART_TX);
    transfer.begin(*serial);
    DEBUG_PRINTLN("UART Communication Started - ESP32 WROOM");
}

bool UARTProtocol::receiveMotorCommand(MotorCommand &cmd, uint8_t &speed) {

    if (transfer.available()) {

        transfer.rxObj(cmd, 0);
        transfer.rxObj(speed, 1);
        
        lastReceivedCommand = cmd;
        lastReceivedSpeed = speed;
        newDataAvailable = true;
        
        sendAcknowledgment(cmd, speed);
        
        return true;
    }
    return false;
}

void UARTProtocol::sendAcknowledgment(MotorCommand cmd, uint8_t speed) {
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