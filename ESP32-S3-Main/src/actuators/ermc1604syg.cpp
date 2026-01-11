#include "ermc1604syg.h"
#include "../config/pins.h"
#include "../config/constants.h"

Display::Display() : lcd(LCD_ADDR, LCD_COLS, LCD_ROWS) {
    lastUpdateTime = 0;
}

bool Display::begin() {
    // Initialize Wire with the project-defined pins
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);
    // Initialize the LCD and turn backlight on
    lcd.init();        // initialize the PCF8574 I/O expander and LCD
    lcd.backlight();   // ensure backlight is ON (you requested always ON)

    lcd.clear();

    // Small splash
    lcd.setCursor(0, 0);
    lcd.print(" SYNAPSE Robot ");
    lcd.setCursor(0, 1);
    lcd.print(" Display Ready ");
    lcd.setCursor(0, 2);
    lcd.print(" I2C Addr: 0x27 ");
    lcd.setCursor(0, 3);
    lcd.print(" Backlight: ON  ");

    delay(2000);
    return true;
}

void Display::clear() {
    lcd.clear();
}

void Display::setCursor(uint8_t col, uint8_t row) {
    if (col >= LCD_COLS) col = LCD_COLS - 1;
    if (row >= LCD_ROWS) row = LCD_ROWS - 1;
    lcd.setCursor(col, row);
}

void Display::print(const char* text) {
    lcd.print(text);
}

void Display::print(String text) {
    lcd.print(text);
}

void Display::print(int value) {
    lcd.print(value);
}

void Display::print(float value, int decimals) {
    lcd.print(value, decimals);
}

void Display::displayStatus(String mode, int battery, float distance, int hr, int spo2) {
    clear();
    setCursor(0, 0);
    // truncate if needed
    String line1 = mode + " Bat:" + String(battery) + "%";
    if (line1.length() > LCD_COLS) line1 = line1.substring(0, LCD_COLS);
    print(line1);

    setCursor(0, 1);
    String line2 = "Following: " + String(distance, 1) + "m";
    if (line2.length() > LCD_COLS) line2 = line2.substring(0, LCD_COLS);
    print(line2);

    setCursor(0, 2);
    String line3 = "HR:" + String(hr) + " SpO2:" + String(spo2) + "%";
    if (line3.length() > LCD_COLS) line3 = line3.substring(0, LCD_COLS);
    print(line3);

    setCursor(0, 3);
    print("Status: Normal");
}

void Display::displayError(String error) {
    clear();
    setCursor(0, 0);
    print("***** ERROR *****");
    setCursor(0, 1);
    print(error.length() > LCD_COLS ? error.substring(0, LCD_COLS) : error);
    setCursor(0, 2);
    print("Check system!");
    setCursor(0, 3);
    print("Function Halted");
}
