#include "tcs3200.h"
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

    // Set frequency scaling to 100%
    digitalWrite(COLOR_S0, HIGH);
    digitalWrite(COLOR_S1, HIGH);

    delay(100);
    calibrate();
}

void ColorSensor::update() {
    unsigned long now = millis();
    if (now - lastReadTime < SENSOR_READ_INTERVAL) return;

    int r = tcs.colorRead('r');
    int g = tcs.colorRead('g');
    int b = tcs.colorRead('b');

    // ---- Store into rolling buffer ----
    rBuffer[bufferIndex] = r;
    gBuffer[bufferIndex] = g;
    bBuffer[bufferIndex] = b;

    bufferIndex = (bufferIndex + 1) % COLOR_AVG_SAMPLES;
    if (bufferIndex == 0) bufferFilled = true;

    int samples = bufferFilled ? COLOR_AVG_SAMPLES : bufferIndex;

    // ---- Compute averaged RGB ----
    long sumR = 0, sumG = 0, sumB = 0;
    for (int i = 0; i < samples; i++) {
        sumR += rBuffer[i];
        sumG += gBuffer[i];
        sumB += bBuffer[i];
    }

    currentColor.red   = sumR / samples;
    currentColor.green = sumG / samples;
    currentColor.blue  = sumB / samples;

    // ---- Dynamic ambient auto-calibration ----
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
    
    Serial.println("ColorSensor calibrated");
    Serial.print("Ambient clear value: ");
    Serial.println(ambientClear);
}

RGBColor ColorSensor::getRGB() {
    return currentColor;
}

ColorType ColorSensor::getColorType() {
    int r = currentColor.red;
    int g = currentColor.green;
    int b = currentColor.blue;

    // ---- Ambient compensation ----
    r = max(0, r - ambientClear);
    g = max(0, g - ambientClear);
    b = max(0, b - ambientClear);

    int sum = r + g + b;
    int maxVal = max(r, max(g, b));
    int minVal = min(r, min(g, b));
    int diff   = maxVal - minVal;

    // ---- Debug output ----
    Serial.print("RGB(avg, adj): ");
    Serial.print(r); Serial.print(", ");
    Serial.print(g); Serial.print(", ");
    Serial.print(b);
    Serial.print(" | Ambient=");
    Serial.print(ambientClear);
    Serial.print(" | Sum=");
    Serial.print(sum);
    Serial.print(" | Diff=");
    Serial.println(diff);

    // ---- BLACK ----
    if (sum <= BLACK_SUM_MAX) {
        Serial.println("Detected: BLACK / UNKNOWN");
        return COLOR_UNKNOWN;
    }

    // ---- WHITE ----
    if (r >= WHITE_R_MIN &&
        g >= WHITE_G_MIN &&
        b >= WHITE_B_MIN &&
        diff <= WHITE_BALANCE_TOLERANCE) {

        Serial.println("Detected: WHITE");
        return COLOR_WHITE;
    }

    // ---- BLUE ----
    if (b >= BLUE_B_MIN && 
        b > r + BLUE_DIFF_MIN && 
        b > g + BLUE_DIFF_MIN) {

        Serial.println("Detected: BLUE");
        return COLOR_BLUE;
    }

    // ---- GREEN ----
    if (g >= GREEN_G_MIN &&
        g > r + GREEN_DIFF_MIN &&
        g > b + GREEN_DIFF_MIN) {

        Serial.println("Detected: GREEN");
        return COLOR_GREEN;
    }

    // ---- RED ----
    if (r >= RED_R_MIN &&
        r > g + RED_DIFF_MIN) {

        Serial.println("Detected: RED");
        return COLOR_RED;
    }

    // ---- PURPLE ----
    if (r >= PURPLE_R_MIN &&
        b >= PURPLE_B_MIN &&
        g <= PURPLE_G_MAX) {

        Serial.println("Detected: PURPLE");
        return COLOR_PURPLE;
    }

    Serial.println("Detected: UNKNOWN");
    return COLOR_UNKNOWN;
}