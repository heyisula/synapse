#ifndef LED_ARRAY_H
#define LED_ARRAY_H

#include <Arduino.h>

enum LEDPattern {
    LED_OFF,
    LED_SOLID,
    LED_BLINK_SLOW,
    LED_BLINK_FAST,
    LED_PULSE
};
class LEDArray {
private:
    uint8_t leftPin;
    uint8_t rightPin;
    LEDPattern leftPattern;
    LEDPattern rightPattern;
    unsigned long lastUpdateTime;
    bool ledState;
    
public:
    LEDArray();
    void begin();
    void update();
    void setPattern(bool isLeft, LEDPattern pattern);
    void setBoth(LEDPattern pattern);
    void setStatus(bool left, bool right);
};

#endif