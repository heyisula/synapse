#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebSerial.h>

class UnifiedLogger : public Print {
public:
    virtual size_t write(uint8_t c) override {
        size_t s = Serial.write(c);
        if (WiFi.status() == WL_CONNECTED) {
            WebSerial.write(c);
        }
        return s;
    }

    virtual size_t write(const uint8_t *buffer, size_t size) override {
        size_t s = Serial.write(buffer, size);
        if (WiFi.status() == WL_CONNECTED) {
            WebSerial.write(buffer, size);
        }
        return s;
    }

    void flush() {
        Serial.flush();
    }
};

extern UnifiedLogger Log;

#endif
