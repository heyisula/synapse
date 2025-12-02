#include "uart_handler.h"

UARTHandler::UARTHandler() 
    : serial(&Serial), receiveBuffer(""), lastCommand(""), 
      lastReceiveTime(0) {}

void UARTHandler::begin(int baudRate) {
    // Use default UART (RX=3, TX=0)
    serial->begin(baudRate);
    serial->setTimeout(10);
    
    Serial.println("UART Handler initialized");
    Serial.print("Baud rate: ");
    Serial.println(baudRate);
    
    lastReceiveTime = millis();
}

void UARTHandler::update() {
    // Read incoming data
    while (serial->available()) {
        char c = serial->read();
        
        if (c == '\n' || c == '\r') {
            if (receiveBuffer.length() > 0) {
                lastCommand = receiveBuffer;
                receiveBuffer = "";
                lastReceiveTime = millis();
            }
        } else if (c >= 32 && c <= 126) {  // Printable characters only
            receiveBuffer += c;
            
            // Prevent buffer overflow
            if (receiveBuffer.length() >= BUFFER_SIZE - 1) {
                Serial.println("ERROR:BUFFER_OVERFLOW");
                receiveBuffer = "";
            }
        }
    }
}

bool UARTHandler::hasCommand() {
    return (lastCommand.length() > 0);
}

String UARTHandler::getCommand() {
    String cmd = lastCommand;
    lastCommand = "";
    return cmd;
}

void UARTHandler::sendResponse(String response) {
    serial->println(response);
    serial->flush();
}

void UARTHandler::sendOK() {
    sendResponse("OK");
}

void UARTHandler::sendError(String error) {
    sendResponse("ERROR:" + error);
}

void UARTHandler::processBuffer() {
    // Additional processing if needed
}