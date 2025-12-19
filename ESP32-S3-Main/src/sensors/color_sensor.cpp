#include "color_sensor.h"
#include "../config/pins.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

ColorSensor::ColorSensor():ambientClear(0) {
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
//20% Feq for best balance between speed and accuracy
    digitalWrite(s0Pin, HIGH);
    digitalWrite(s1Pin, LOW);
}

void ColorSensor::calibrate() {
    Serial.println("Calibrating for ambient light...");
    const int N = 10;
    long sum = 0;

    for (int i = 0; i < N; ++i) {
        // Read the clear channel – same as any colour, but with no filter
        digitalWrite(s2Pin, LOW);   // Red filter on
        digitalWrite(s3Pin, LOW);   // Blue filter off (clear)
        delay(10);
        sum += pulseIn(outPin, LOW, 50000);
    }

    ambientClear = sum / N;
    Serial.print("Ambient clear baseline set to: ");
    Serial.println(ambientClear);
    delay(2000);
}

int ColorSensor::readColor(bool r, bool g, bool b) {
    digitalWrite(s2Pin, r ? LOW : HIGH);
    digitalWrite(s3Pin, b ? LOW : HIGH);
    delay(10);

    unsigned long duration = pulseIn(outPin, LOW, 50000);
    if(duration == 0){
        return 0;
    }
    
    int value = map(duration, 10, 1000, 255, 0);
    return constrain(value, 0, 255);
}

void ColorSensor::update() {
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= SENSOR_READ_INTERVAL) {
        int rawRed   = readColor(true,  false, false);
        int rawGreen = readColor(false, true,  false);
        int rawBlue  = readColor(false, false, true );

        // Subtracting the ambient baseline
        currentColor.red   = constrain(rawRed   - ambientClear, 0, 255);
        currentColor.green = constrain(rawGreen - ambientClear, 0, 255);
        currentColor.blue  = constrain(rawBlue  - ambientClear, 0, 255);
        lastReadTime = currentTime;
    }
}

RGBColor ColorSensor::getRGB() {
    return currentColor;
}

ColorType ColorSensor::getColorType() {
    // White
    if(currentColor.red >= WHITE_R_MIN && 
       currentColor.green >= WHITE_G_MIN && 
       currentColor.blue >= WHITE_B_MIN) {
        return COLOR_WHITE;
    }
    // Blue
    if(currentColor.blue >= BLUE_B_MIN && 
       currentColor.red <= BLUE_R_MAX) {
        return COLOR_BLUE;
    }
    // Red
    if(currentColor.red >= RED_R_MIN && 
       currentColor.green <= RED_G_MAX) {
        return COLOR_RED;
    }
    // Green
    if (currentColor.green >= GREEN_G_MIN &&      
        currentColor.red   <= GREEN_R_MIN &&     
        currentColor.blue  <= GREEN_B_MAX) {     
        return COLOR_GREEN;          
    }
    // Black
    int sum = currentColor.red + currentColor.green + currentColor.blue;
    if(sum <= BLACK_SUM_MAX) {
        return COLOR_BLACK;
    }
    
    return COLOR_UNKNOWN;
}

bool ColorSensor::isRed(){ 
    return getColorType() == COLOR_RED;
}
bool ColorSensor::isGreen(){ 
    return getColorType() == COLOR_GREEN;
}
bool ColorSensor::isBlack(){ 
    return getColorType() == COLOR_BLACK;
}
bool ColorSensor::isUnknown(){
    return getColorType() == COLOR_UNKNOWN;
}

bool ColorSensor::isWhiteCoat() {
    return getColorType() == COLOR_WHITE;
}

bool ColorSensor::isBlueScrubs() {
    return getColorType() == COLOR_BLUE;
}

bool ColorSensor::isBlueDominant(){
    // Blue is dominant when it’s high enough and red is low.
    return currentColor.blue  >= BLUE_B_MIN &&
           currentColor.red   <= BLUE_R_MAX;
}

bool ColorSensor::isWhiteDominant() {
    // All three channels must exceed their white thresholds
    return currentColor.red   >= WHITE_R_MIN &&
           currentColor.green >= WHITE_G_MIN &&
           currentColor.blue  >= WHITE_B_MIN;
}

bool ColorSensor::isPureWhite(){
    // Pure white means all channels are high and none dominates.
    if (!isWhiteDominant()) return false;

    // If any other channel is significantly higher than the others,
    // we’re not “pure” – this is a very simple heuristic:
    int maxVal = max(max(currentColor.red, currentColor.green),
                     currentColor.blue);
    int minVal = min(min(currentColor.red, currentColor.green),
                     currentColor.blue);

    return (maxVal - minVal) <= 20;   // tolerance of 20 out of 255
}