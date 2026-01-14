#include "automatic_lighting.h"

AutomaticLighting::AutomaticLighting(LightSensor* sensor, LEDArray* leds)
    : lightSensor(sensor), ledArray(leds), isRunning(false), lastUpdateTime(0), updateInterval(1000) {}

void AutomaticLighting::begin() {
    // Initialization already handled by sensor/led begin()
}

void AutomaticLighting::start() {
    isRunning = true;
}

void AutomaticLighting::stop() {
    isRunning = false;
    ledArray->setBothBrightness(0);
}

void AutomaticLighting::update() {
    if (!isRunning) return;

    unsigned long now = millis();
    if (now - lastUpdateTime >= updateInterval) {
        lastUpdateTime = now;
        
        int darkness = lightSensor->getPathDarkness(); // 0 is bright, high is dark
        uint8_t brightness = mapDarknessToLED(darkness);
        
        ledArray->setBothBrightness(brightness);
    }
}

uint8_t AutomaticLighting::mapDarknessToLED(int darkness) {
    // Adjust mapping based on sensor range
    return (uint8_t)constrain(map(darkness, 500, 4000, 0, 255), 0, 255);
}
