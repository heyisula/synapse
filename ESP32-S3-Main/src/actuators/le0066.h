#ifndef LED_ARRAY_H
#define LED_ARRAY_H

#include <Arduino.h>

class LEDArray {
private:
    uint8_t leftPin;
    uint8_t rightPin;
    uint8_t leftBrightness;
    uint8_t rightBrightness;
    
public:
    LEDArray();
    void begin();

    void setBrightness(uint8_t left, uint8_t right);
    void setBothBrightness(uint8_t brightness);
    void update();
    void setStatus(bool left, bool right);

    void controlFromFirebase(int lightadj_left, int lightadj_right);
};

#endif