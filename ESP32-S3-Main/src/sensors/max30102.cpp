#include "max30102.h"
#include "../config/constants.h"
#include "../config/thresholds.h"
#include "../config/pins.h"

HeartRateSensor::HeartRateSensor()
    : heartRate(0),
      spO2(0),
      temperature(0),
      fingerDetected(false),
      lastReadTime(0),
      lastBeat(0),
      beatsPerMinute(0),
      rateSpot(0),
      beatAvg(0),
      bufferIndex(0),
      bufferFilled(false)
{
    memset(rates, 0, sizeof(rates));
    memset(irBuffer, 0, sizeof(irBuffer));
    memset(redBuffer, 0, sizeof(redBuffer));
}

bool HeartRateSensor::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);

    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("MAX30102 not found! Check wiring/power.");
        return false;
    }
    
    // Configure sensor with optimal settings for heart rate and SpO2
    byte ledBrightness = 60;   // LED brightness (0-255)
    byte sampleAverage = 4;    // Average 4 samples
    byte ledMode = 2;          // Red + IR mode for SpO2
    byte sampleRate = 100;     // 100 samples per second
    int pulseWidth = 411;      // Pulse width in microseconds
    int adcRange = 4096;       // ADC range
    
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
    
    // Set specific LED amplitudes
    particleSensor.setPulseAmplitudeRed(0x0A);
    particleSensor.setPulseAmplitudeGreen(0);
    
    // Enable temperature reading
    particleSensor.enableDIETEMPRDY();
    
    Serial.println("MAX30102 initialized successfully");
    Serial.println("Place your finger on the sensor with steady pressure.");
    
    return true;
}

void HeartRateSensor::update() {
    unsigned long currentTime = millis();
    
    // Update at ~20Hz for heart rate, less frequently for temperature
    if (currentTime - lastReadTime < 50) return;
    lastReadTime = currentTime;
    
    // Check if new data is available
    while (particleSensor.available() == false) {
        particleSensor.check();
    }
    
    // Read IR and Red values
    long irValue = particleSensor.getIR();
    long redValue = particleSensor.getRed();
    particleSensor.nextSample();
    
    // Check if finger is detected (IR value threshold)
    fingerDetected = (irValue > 10000);
    
    if (fingerDetected) {
        // ===== HEART RATE CALCULATION =====
        if (checkForBeat(irValue) == true) {
            // We sensed a beat!
            long delta = currentTime - lastBeat;
            lastBeat = currentTime;
            
            beatsPerMinute = 60 / (delta / 1000.0);
            
            // Only accept reasonable heart rate values
            if (beatsPerMinute < 255 && beatsPerMinute > 20) {
                rates[rateSpot++] = (byte)beatsPerMinute;
                rateSpot %= RATE_SIZE;
                
                // Calculate average heart rate
                beatAvg = 0;
                for (byte i = 0; i < RATE_SIZE; i++) {
                    beatAvg += rates[i];
                }
                beatAvg /= RATE_SIZE;
                
                heartRate = beatAvg;
            }
        }
        
        // ===== SPO2 CALCULATION =====
        // Store values in buffer
        irBuffer[bufferIndex] = irValue;
        redBuffer[bufferIndex] = redValue;
        bufferIndex++;
        
        if (bufferIndex >= SPO2_BUFFER_SIZE) {
            bufferIndex = 0;
            bufferFilled = true;
        }
        
        // Calculate SpO2 when buffer is filled
        if (bufferFilled) {
            calculateSpO2();
        }
        
        // ===== TEMPERATURE READING =====
        // Read temperature less frequently (every ~2 seconds)
        static unsigned long lastTempRead = 0;
        if (currentTime - lastTempRead > 2000) {
            readTemperature();
            lastTempRead = currentTime;
        }
        
    } else {
        // No finger detected - reset values
        heartRate = 0;
        spO2 = 0;
        beatAvg = 0;
        beatsPerMinute = 0;
        bufferIndex = 0;
        bufferFilled = false;
        memset(rates, 0, sizeof(rates));
    }
}

void HeartRateSensor::calculateSpO2() {
    // Calculate DC and AC components for IR and Red
    uint32_t irDC = 0;
    uint32_t redDC = 0;
    int32_t irAC = 0;
    int32_t redAC = 0;
    
    // Calculate DC (average)
    for (byte i = 0; i < SPO2_BUFFER_SIZE; i++) {
        irDC += irBuffer[i];
        redDC += redBuffer[i];
    }
    irDC /= SPO2_BUFFER_SIZE;
    redDC /= SPO2_BUFFER_SIZE;
    
    // Calculate AC (variance)
    for (byte i = 0; i < SPO2_BUFFER_SIZE; i++) {
        irAC += abs((int32_t)irBuffer[i] - (int32_t)irDC);
        redAC += abs((int32_t)redBuffer[i] - (int32_t)redDC);
    }
    irAC /= SPO2_BUFFER_SIZE;
    redAC /= SPO2_BUFFER_SIZE;
    
    // Calculate SpO2 using ratio of ratios
    if (irDC > 0 && redDC > 0 && irAC > 0) {
        float ratio = ((float)redAC / (float)redDC) / ((float)irAC / (float)irDC);
        
        // Empirical formula for SpO2 calculation
        // This is a simplified version - more accurate algorithms exist
        spO2 = (int)(110 - 25 * ratio);
        
        // Constrain to realistic values
        spO2 = constrain(spO2, SPO2_MIN, SPO2_MAX);
    }
}
bool HeartRateSensor::checkForBeat(long irValue) {
    bool beatDetected = false;
    
    // Simple peak detection
    // A beat is detected when IR value crosses above threshold
    if (irValue > BEAT_THRESHOLD) {
        if (irValue > lastIRValue) {
            long delta = irValue - lastIRValue;
            if (delta > 500) {  // Significant increase
                beatDetected = true;
            }
        }
    }
    
    lastIRValue = irValue;
    return beatDetected;
}
void HeartRateSensor::readTemperature() {
    temperature = particleSensor.readTemperature();
}

int HeartRateSensor::getHeartRate() {
    return heartRate;
}

int HeartRateSensor::getSpO2() {
    return spO2;
}

float HeartRateSensor::getTemperature() {
    return temperature;
}

float HeartRateSensor::getTemperatureF() {
    return particleSensor.readTemperatureF();
}

bool HeartRateSensor::isFingerDetected() {
    return fingerDetected;
}

bool HeartRateSensor::isValid() {
    return fingerDetected && 
           heartRate >= HR_MIN && 
           heartRate <= HR_MAX && 
           spO2 >= SPO2_MIN && 
           spO2 <= SPO2_MAX;
}

bool HeartRateSensor::monitorHeartRate(bool heartrate_start, int& hr, int& sp02) {
    if (heartrate_start) {

        if (!isHeartRateMonitoringActive) {
            isHeartRateMonitoringActive = true;
            Serial.println("=== Heart Rate monitoring STARTED ===");
            Serial.println("Place your finger on the sensor...");
            lastHeartRate = 0;
            lastSpO2 = 0;
        }
        
        update();
        
        if (!fingerDetected) {
            Serial.println("⚠ No finger detected - place finger on sensor");
            hr = 0;
            sp02 = 0;
            return false;
        }
        
        int currentHR = getHeartRate();
        int currentSpO2 = getSpO2();
        
        if (isValid()) {
            hr = currentHR;
            sp02 = currentSpO2;
            
            if (currentHR != lastHeartRate || currentSpO2 != lastSpO2) {
                lastHeartRate = currentHR;
                lastSpO2 = currentSpO2;
                
                Serial.println("┌─────────────────────────");
                Serial.print("│ Heart Rate: ");
                Serial.print(hr);
                Serial.println(" BPM");
                Serial.print("│ SpO2: ");
                Serial.print(sp02);
                Serial.println("%");
                Serial.println("└─────────────────────────");
            }
            
            return true;
        } else {
            hr = 0;
            sp02 = 0;
            return false;
        }
        
    } else {

        if (isHeartRateMonitoringActive) {
            isHeartRateMonitoringActive = false;
            Serial.println("=== Heart Rate monitoring STOPPED ===");
        }
        
        hr = 0;
        sp02 = 0;
        lastHeartRate = 0;
        lastSpO2 = 0;
        
        return false;
    }
}