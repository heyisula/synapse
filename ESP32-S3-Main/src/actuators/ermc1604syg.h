#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class Display {
private:
    const uint8_t LCD_ADDR = 0x27;
    const uint8_t LCD_COLS = 16;
    const uint8_t LCD_ROWS = 4;
    unsigned long lastUpdateTime;
    LiquidCrystal_I2C lcd;

public:
    Display();
    
    bool begin();
    void clear();
    void setCursor(uint8_t col, uint8_t row);

    void print(const char* text);
    void print(String text);
    void print(int value);
    void print(float value, int decimals = 1);

    void displayStatus(int battery, float temp, float humidity, int modeNum);
    void displayError(String error);
};

#endif
