#include "uart.h"
#include "../config/pins.h"
#include "../config/constants.h"

UARTProtocol::UARTProtocol() {
    serial = &Serial1;
    lastSendTime = 0;
}

void UARTProtocol::begin() {
    serial->begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX, UART_TX);
    transfer.begin(*serial);
}


void UARTProtocol::sendMotorCommand(MotorCommand cmd, uint8_t speed) {
    // Put command and speed into the transmit buffer
    transfer.txObj(cmd, 0);
    transfer.txObj(constrain(speed, 0, 100), 1);

    // Send the packet over Serial1
    transfer.sendData(2);  // no parameters needed
}


void UARTProtocol::sendEmergencyStop() {
    sendMotorCommand(CMD_EMERGENCY_STOP, 0);
}

bool UARTProtocol::receiveAcknowledgment(MotorCommand &cmd, uint8_t &speed) {
    // Check if a full packet has been received
    if (transfer.available()) {
        // Read command and speed from receive buffer
        transfer.rxObj(cmd, 0);
        transfer.rxObj(speed, 1);
        return true;  // acknowledgment received
    }
    return false;  // no data yet
}