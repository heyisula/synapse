#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <Arduino.h>

enum ColorType {
    COLOR_WHITE,
    COLOR_BLUE,
    COLOR_RED,
    COLOR_BLACK,
    COLOR_GREEN,
    COLOR_UNKNOWN
};

struct RGBColor {
    int red;
    int green;
    int blue;
};

class ColorSensor {
private:
    uint8_t s0Pin, s1Pin, s2Pin, s3Pin, outPin;
    RGBColor currentColor;
    unsigned long lastReadTime;
    int ambientClear;
    int readColor(bool r, bool g, bool b);
    
public:
    ColorSensor();
    void begin();
    void calibrate();
    void update();
    RGBColor getRGB();
    ColorType getColorType();
    bool isWhiteCoat();
    bool isBlueScrubs();

    bool isRed();
    bool isGreen();
    bool isBlack();
    bool isUnknown();
    bool isBlueDominant();
    bool isWhiteDominant();
    bool isPureWhite();
};

#endif