#include "color_sensor.h"
#include "../config/pins.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

ColorSensor::ColorSensor() {
    s0Pin = COLOR_S0;
    s1Pin = COLOR_S1;
    s2Pin = COLOR_S2;
    s3Pin = COLOR_S3;
    outPin = COLOR_OUT;
    
    currentColor = {0, 0, 0};
    lastReadTime = 0;
}

void ColorSensor::begin() {
    pinMode(s0Pin, OUTPUT);
    pinMode(s1Pin, OUTPUT);
    pinMode(s2Pin, OUTPUT);
    pinMode(s3Pin, OUTPUT);
    pinMode(outPin, INPUT);
    
    // Set frequency scaling to 20%
    digitalWrite(s0Pin, HIGH);
    digitalWrite(s1Pin, LOW);
}

void ColorSensor::calibrate() {
    Serial.println("Color sensor calibration...");
    delay(2000);
}

int ColorSensor::readColor(bool r, bool g, bool b) {
    // Set photodiode filter
    digitalWrite(s2Pin, r ? LOW : HIGH);
    digitalWrite(s3Pin, b ? LOW : HIGH);
    
    delay(10);
    
    // Read frequency (inverse of period)
    unsigned long duration = pulseIn(outPin, LOW, 50000);
    
    if(duration == 0) return 0;
    
    // Convert to 0-255 range (approximate)
    int value = map(duration, 10, 1000, 255, 0);
    return constrain(value, 0, 255);
}

void ColorSensor::update() {
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= SENSOR_READ_INTERVAL) {
        currentColor.red = readColor(true, false, false);
        currentColor.green = readColor(false, true, false);
        currentColor.blue = readColor(false, false, true);
        
        lastReadTime = currentTime;
    }
}

RGBColor ColorSensor::getRGB() {
    return currentColor;
}

ColorType ColorSensor::getColorType() {
    // White detection
    if(currentColor.red >= WHITE_R_MIN && 
       currentColor.green >= WHITE_G_MIN && 
       currentColor.blue >= WHITE_B_MIN) {
        return COLOR_WHITE;
    }
    
    // Blue detection
    if(currentColor.blue >= BLUE_B_MIN && 
       currentColor.red <= BLUE_R_MAX) {
        return COLOR_BLUE;
    }
    
    // Red detection
    if(currentColor.red >= RED_R_MIN && 
       currentColor.green <= RED_G_MAX) {
        return COLOR_RED;
    }
    
    // Black detection
    int sum = currentColor.red + currentColor.green + currentColor.blue;
    if(sum <= BLACK_SUM_MAX) {
        return COLOR_BLACK;
    }
    
    return COLOR_UNKNOWN;
}

bool ColorSensor::isWhiteCoat() {
    return getColorType() == COLOR_WHITE;
}

bool ColorSensor::isBlueScrubs() {
    return getColorType() == COLOR_BLUE;
}