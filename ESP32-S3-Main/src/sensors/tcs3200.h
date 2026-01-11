#ifndef TCS3200_H
#define TCS3200_H

#include <Arduino.h>
#include <tcs3200.h>
#include "../config/thresholds.h"

enum ColorType {
    COLOR_WHITE,    //Patients
    COLOR_BLUE,     //Surgeon | Doctor
    COLOR_RED,      //Medical Students
    COLOR_PURPLE,   //Minor Staff
    COLOR_GREEN,    //Nurses
    COLOR_UNKNOWN
};

struct RGBColor {
    int red;
    int green;
    int blue;
};

class ColorSensor {
private:
    tcs3200 tcs;
    RGBColor currentColor;
    unsigned long lastReadTime;
    int ambientClear;
    
    // Temporal averaging buffers
    int rBuffer[COLOR_AVG_SAMPLES];
    int gBuffer[COLOR_AVG_SAMPLES];
    int bBuffer[COLOR_AVG_SAMPLES];
    uint8_t bufferIndex;
    bool bufferFilled;


public:
    ColorSensor();
    void begin();
    void calibrate();
    void update();
    RGBColor getRGB();
    ColorType getColorType();
};

#endif