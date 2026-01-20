#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>

class Battery {
public:
    Battery();

    void begin();
    float readVoltage();      
    int   readPercentage();   

    int getBatteryLevel();
    int getVoltageInt();
    void getBatteryData(int& battery, float& voltage); // Voltage in Volts

private:
    uint8_t _adcPin;

    const float R1 = 100000.0;   // 100kΩ
    const float R2 = 33000.0;    // 33kΩ


    const float MIN_VOLTAGE = 12.0;
    const float MAX_VOLTAGE = 16.8;

    /*
        Battery + ---- R1 ----+---- R2 ---- GND
                              |
                        ADC Pin
    */
};

#endif
