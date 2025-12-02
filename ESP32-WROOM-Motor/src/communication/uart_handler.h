#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include <Arduino.h>

class UARTHandler {
private:
    HardwareSerial* serial;
    String receiveBuffer;
    String lastCommand;
    unsigned long lastReceiveTime;
    
    static const int BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE];
    
public:
    UARTHandler();
    void begin(int baudRate = 115200);
    void update();
    
    bool hasCommand();
    String getCommand();
    
    void sendResponse(String response);
    void sendOK();
    void sendError(String error);
    
    unsigned long getLastReceiveTime() { return lastReceiveTime; }
    
private:
    void processBuffer();
};

#endif