#include <Arduino.h>

// Pin definitions from pins.h
#define S1 16
#define S2 4
#define S3 5
#define S4 6
#define S5 7

// Arrays for pins and calibration values
int sensorPins[5] = {S1, S2, S3, S4, S5};
int whiteVal[5]; // max readings (white)
int blackVal[5]; // min readings (black)

// Forward declaration
int readSensor(int pin);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  analogReadResolution(12);       // 12-bit ADC
  analogSetAttenuation(ADC_11db); // full range

  // Set sensor pins as input
  for (int i = 0; i < 5; i++) pinMode(sensorPins[i], INPUT);

  Serial.println("=== Line Sensor Auto-Calibration ===");
  delay(1000);

  // Auto calibration for WHITE background
  Serial.println("Place sensors over WHITE surface...");
  delay(3000);
  for (int i = 0; i < 5; i++) {
    whiteVal[i] = readSensor(sensorPins[i]);
  }

  // Auto calibration for BLACK line
  Serial.println("Place sensors over BLACK line...");
  delay(3000);
  for (int i = 0; i < 5; i++) {
    blackVal[i] = readSensor(sensorPins[i]);
  }

  Serial.println("Calibration done!");
  Serial.print("White values: ");
  for (int i = 0; i < 5; i++) Serial.print(whiteVal[i]), Serial.print("\t");
  Serial.println();
  
  Serial.print("Black values: ");
  for (int i = 0; i < 5; i++) Serial.print(blackVal[i]), Serial.print("\t");
  Serial.println("\n\nStarting line detection:");
  delay(1000);
}

void loop() {
  int lineState[5];

  // Read and map sensor values
  for (int i = 0; i < 5; i++) {
    int raw = readSensor(sensorPins[i]);
    
    // Normalize 0-1000
    int normalized = map(raw, blackVal[i], whiteVal[i], 0, 1000);
    normalized = constrain(normalized, 0, 1000);
    
    // Detect line: 0 = black, 1 = white
    lineState[i] = (normalized > 500) ? 1 : 0;
  }

  // Print line detection
  for (int i = 0; i < 5; i++) {
    Serial.print(lineState[i]);
    Serial.print("\t");
  }
  Serial.println();

  delay(100);
}

// Function to read sensor with smoothing
int readSensor(int pin) {
  const int SAMPLES = 5;
  long sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += analogRead(pin);
    delay(1);
  }
  return sum / SAMPLES;
}
