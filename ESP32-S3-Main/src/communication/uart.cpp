#include "uart.h"
#include "../config/pins.h"
#include "../config/constants.h"

UARTProtocol::UARTProtocol() {
    serial = &Serial1;
    lastSendTime = 0;
    lastSentCommand = CMD_STOP;
    isWaitingForAck = false;
    lastAckTime = 0;
    initialized = false;
}

void UARTProtocol::begin() {
    serial->begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX, UART_TX);
    transfer.begin(*serial);
    initialized = true;
}


void UARTProtocol::sendMotorCommand(MotorCommand cmd, uint8_t speed) {
    if (!initialized) return;

    uint8_t cmdValue = (uint8_t)cmd;
    uint8_t speedValue = constrain(speed, 0, 100);
    
    // DEBUG: Visualize outgoing command
    Serial.print("-> MOTOR_TX: Cmd ");
    Serial.print(cmdValue);
    Serial.print(" | Spd ");
    Serial.println(speedValue);

    // Put command and speed into distinct slots

    // Put command and speed into distinct slots
    transfer.txObj(cmdValue, 0);      
    transfer.txObj(speedValue, 1);    

    transfer.sendData(2); 

    // Update tracking state
    lastSentCommand = cmd;
    lastSendTime = millis();
    isWaitingForAck = true;
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

        // Verify if this matches what we sent
        if (isWaitingForAck && cmd == lastSentCommand) {
            isWaitingForAck = false;
            lastAckTime = millis();
            return true;  // Correct acknowledgment received
        }
        
        return true; // Received some packet, even if not the specific ACK we waited for
    }

    // Check for timeout
    if (isWaitingForAck && (millis() - lastSendTime > 500)) {
        isWaitingForAck = false; 
    }

    return false;  // no data yet
}