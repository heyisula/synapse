#include "environmental_mode.h"
#include "../config/thresholds.h"

EnvironmentalMode::EnvironmentalMode(Environmental* env, LightSensor* light, Display* disp, Buzzer* bz) {
    envSensor = env;
    lightSensor = light;
    display = disp;
    buzzer = bz;
    state = ENV_OPTIMAL;
}

void EnvironmentalMode::begin() {
    state = ENV_OPTIMAL;
}

void EnvironmentalMode::update() {
    envSensor->update();
    lightSensor->update();
    checkConditions();
}

void EnvironmentalMode::checkConditions() {
    float temp = envSensor->getTemperature();
    float humidity = envSensor->getHumidity();
    bool lightOK = lightSensor->isPathVisible();
    
    if(temp < TEMP_MIN || temp > TEMP_MAX) {
        state = ENV_WARNING;
        buzzer->playTone(TONE_WARNING);
    } else if(humidity < HUMIDITY_MIN || humidity > HUMIDITY_MAX) {
        state = ENV_WARNING;
        buzzer->playTone(TONE_WARNING);
    } else if(!lightOK) {
        state = ENV_WARNING;
    } else {
        state = ENV_OPTIMAL;
    }
}

void EnvironmentalMode::displayReadings() {
    display->clear();
    display->setCursor(0, 0);
    display->print("Environment");
    display->setCursor(0, 1);
    display->print("T:");
    display->print(envSensor->getTemperature(), 1);
    display->print("C H:");
    display->print((int)envSensor->getHumidity());
    display->print("%");
}