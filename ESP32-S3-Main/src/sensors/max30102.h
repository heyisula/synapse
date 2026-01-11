#ifndef MAX30102_H
#define MAX30102_H

#include <Arduino.h>
#include <MAX30105.h>
#include "heartRate.h"

class HeartRateSensor {
private:
    MAX30105 particleSensor;

    int heartRate;
    int spO2;
    float temperature;
    bool fingerDetected;

    unsigned long lastReadTime;
    long  lastBeat;
    float beatsPerMinute;

    static constexpr byte RATE_SIZE = 4;
    byte rates[RATE_SIZE];
    byte rateSpot;
    int beatAvg;
    
    // SpO2 calculation buffers
    static constexpr byte SPO2_BUFFER_SIZE = 25;
    uint32_t irBuffer[SPO2_BUFFER_SIZE];
    uint32_t redBuffer[SPO2_BUFFER_SIZE];
    byte bufferIndex;
    bool bufferFilled;
    
    // Helper methods
    void calculateSpO2();
    void readTemperature();

    const byte BEAT_THRESHOLD = 50;
    long lastIRValue;
    bool checkForBeat(long irValue);

    
    //static constexpr uint8_t MAX30102_ADDR = 0x57;
    
public:
    HeartRateSensor();
    bool begin();
    void update();
    int getHeartRate();
    int getSpO2();
    float getTemperature();
    float getTemperatureF();
    bool isFingerDetected();
    bool isValid();
};

#endif
