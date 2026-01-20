#include "tcs3200.h"
#include "../utils/logger.h"
#include "../config/pins.h"
#include "../config/constants.h"
#include "../config/thresholds.h"

ColorSensor::ColorSensor()
    : tcs(COLOR_S0, COLOR_S1, COLOR_S2, COLOR_S3, COLOR_OUT),
      lastReadTime(0),
      ambientClear(0),
      bufferIndex(0),
      bufferFilled(false)
{
    currentColor = {0, 0, 0};

    memset(rBuffer, 0, sizeof(rBuffer));
    memset(gBuffer, 0, sizeof(gBuffer));
    memset(bBuffer, 0, sizeof(bBuffer));
}

void ColorSensor::begin() {
    pinMode(COLOR_S0, OUTPUT);
    pinMode(COLOR_S1, OUTPUT);
    pinMode(COLOR_S2, OUTPUT);
    pinMode(COLOR_S3, OUTPUT);
    pinMode(COLOR_OUT, INPUT);

    digitalWrite(COLOR_S0, HIGH);
    digitalWrite(COLOR_S1, LOW);

    delay(100);
    calibrate();
}

void ColorSensor::update() {
    unsigned long now = millis();
    if (now - lastReadTime < SENSOR_READ_INTERVAL) return;

    int r = tcs.colorRead('r');
    int g = tcs.colorRead('g');
    int b = tcs.colorRead('b');
    
    // Safety check: If all readings are basically zero, the sensor might be in the dark so skipping
    if (r + g + b < 5) {
        return;
    }

    //Store into rolling buffer
    rBuffer[bufferIndex] = r;
    gBuffer[bufferIndex] = g;
    bBuffer[bufferIndex] = b;

    bufferIndex = (bufferIndex + 1) % COLOR_AVG_SAMPLES;
    if (bufferIndex == 0) bufferFilled = true;

    int samples = bufferFilled ? COLOR_AVG_SAMPLES : bufferIndex;

    //Compute averaged RGB
    long sumR = 0, sumG = 0, sumB = 0;
    for (int i = 0; i < samples; i++) {
        sumR += rBuffer[i];
        sumG += gBuffer[i];
        sumB += bBuffer[i];
    }

    currentColor.red   = sumR / samples;
    currentColor.green = sumG / samples;
    currentColor.blue  = sumB / samples;

    //Dynamic ambient auto-calibration
    int avgSum = currentColor.red + currentColor.green + currentColor.blue;

    // Only adapt when no strong color detected
    if (avgSum < ambientClear + AMBIENT_UPDATE_THRESHOLD) {
        ambientClear = (1.0f - AMBIENT_ADAPT_RATE) * ambientClear +
                        AMBIENT_ADAPT_RATE * (avgSum / 3);
    }

    lastReadTime = now;
}

void ColorSensor::calibrate() {
    const int samples = 10;
    long sumR = 0, sumG = 0, sumB = 0;

    for (int i = 0; i < samples; i++) {
        sumR += tcs.colorRead('r');
        sumG += tcs.colorRead('g');
        sumB += tcs.colorRead('b');
        delay(50);
    }
    ambientClear = (sumR + sumG + sumB) / (samples * 3);
    
    Log.println("ColorSensor calibrated");
    Log.print("Ambient clear value: ");
    Log.println(ambientClear);
}

RGBColor ColorSensor::getRGB() {
    return currentColor;
}

ColorType ColorSensor::getColorType() {
    int r = currentColor.red;
    int g = currentColor.green;
    int b = currentColor.blue;

    // Ambient compensation
    r = max(0, r - ambientClear);
    g = max(0, g - ambientClear);
    b = max(0, b - ambientClear);

    int sum = r + g + b;
    
    Serial.print("RGB(adj): R=");
    Serial.print(r); Serial.print(" G=");
    Serial.print(g); Serial.print(" B=");
    Serial.print(b); Serial.print(" SUM=");
    Serial.println(sum);

    //TOO DARK
    if (sum <= BLACK_SUM_MAX) {
        Serial.println("-> Too dark to read");
        return COLOR_UNKNOWN;
    }

    // Calculate normalized ratios (0.0 to 1.0)
    float rRatio = (float)r / sum;
    float gRatio = (float)g / sum;
    float bRatio = (float)b / sum;
    
    Serial.print("Ratios: R=");
    Serial.print(rRatio, 3); Serial.print(" G=");
    Serial.print(gRatio, 3); Serial.print(" B=");
    Serial.println(bRatio, 3);

    //RED: R is strongly dominant
    // R=21-40, G=5-9, B=7-12

    if (r >= RED_R_MIN && 
        r > g + RED_DIFF_MIN && 
        r > b + RED_DIFF_MIN &&
        rRatio > 0.60) { 
        Serial.println("-> RED");
        return COLOR_RED;
    }

    //BLUE: B is dominant
    // R=7-16, G=13-32, B=20-47
    if (b >= BLUE_B_MIN && 
        b > r + BLUE_DIFF_MIN && 
        b > g + 3 &&
        bRatio > 0.40) {  
        Serial.println("-> BLUE");
        return COLOR_BLUE;
    }
    //WHITE: Balanced RGB
    // R=27-52, G=26-52, B=33-62
    if (r >= WHITE_R_MIN && 
        g >= WHITE_G_MIN && 
        b >= WHITE_B_MIN) {
        
        int maxVal = max(r, max(g, b));
        int minVal = min(r, min(g, b));
        int diff = maxVal - minVal;
        
        if (diff <= WHITE_BALANCE_TOLERANCE) {
            Serial.println("-> WHITE");
            return COLOR_WHITE;
        }
    }

    //GREEN: G is dominant
    //  R=7-19, G=9-27, B=6-16

    if (g >= GREEN_G_MIN && 
        g > r + GREEN_DIFF_MIN && 
        g > b + GREEN_DIFF_MIN && 
        gRatio > 0.35) {  
        Serial.println("-> GREEN");
        return COLOR_GREEN;
    }

    Serial.println("-> UNKNOWN");
    return COLOR_UNKNOWN;
}

String ColorSensor::colorTypeToString(ColorType type) {
    switch(type) {
        case COLOR_WHITE:   return "WHITE";
        case COLOR_BLUE:    return "BLUE";
        case COLOR_RED:     return "RED";
        case COLOR_GREEN:   return "GREEN";
        case COLOR_UNKNOWN: return "UNKNOWN";
        default:            return "UNKNOWN";
    }
}

String ColorSensor::monitorColor(bool colour_start) {
    if (colour_start) {
        if (!isColorSensingActive) {
            isColorSensingActive = true;
            Serial.println("Color sensing STARTED");
            lastDetectedColor = "UNKNOWN";
        }
        update();
        
        ColorType detectedType = getColorType();
        String detectedColorStr = colorTypeToString(detectedType);
        
        if (detectedColorStr != lastDetectedColor) {
            lastDetectedColor = detectedColorStr;
            Serial.print("Color changed to: ");
            Serial.println(lastDetectedColor);
        }
        
        return lastDetectedColor;
    } 
    else {
        if (isColorSensingActive) {
            isColorSensingActive = false;
            Serial.println("Color sensing STOPPED");
            lastDetectedColor = "UNKNOWN";
        }
        
        return "UNKNOWN";
    }
}