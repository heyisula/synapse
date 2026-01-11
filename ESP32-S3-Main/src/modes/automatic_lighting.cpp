#include "automatic_lighting.h"

AutomaticLighting::AutomaticLighting(LightSensor* sensor, LEDArray* leds)
    : lightSensor(sensor), ledArray(leds), isRunning(false), lastUpdateTime(0), updateInterval(100) {}

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
    // LightSensor::getPathDarkness() returns:
    // 0 = Bright (Both sensors see light) -> LED OFF (0)
    // 128 = Dim (One sensor sees dark) -> LED MID (128)
    // 255 = Dark (Both sensors see dark) -> LED FULL (255)
    return (uint8_t)constrain(darkness, 0, 255);
}
