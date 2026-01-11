#include <Arduino.h>
#include "sensors/max30102.h"
#include "sensors/am2303.h"
#include "sensors/lm393.h"
#include "actuators/ermc1604syg.h"
#include "actuators/sfm27.h"
#include "modes/monitoring.h"

// Create sensor and actuator objects
HeartRateSensor hrSensor;
Environmental envSensor;
LightSensor lightSensor;
Display display;
Buzzer buzzer;

// Monitoring system instance
MonitoringSystem monitoring(&hrSensor, &envSensor, &lightSensor, &display, &buzzer);

void setup() {
    Serial.begin(115200);
    while (!Serial) ; // wait for Serial monitor

    Serial.println("=== Monitoring System Test ===");

    // Initialize system
    if (!monitoring.initialize()) {
        Serial.println("Initialization failed. Check sensors.");
        while (1) delay(1000);
    }

    // Calibrate sensors
    monitoring.calibrateSensors();

    // Start continuous monitoring
    monitoring.startMonitoring();
}

void loop() {
    // Update monitoring system
    monitoring.update();

    // Optional: debug output via Serial
    MonitoringData data = monitoring.getCurrentData();

    Serial.print("HR: "); Serial.print(data.heartRate);
    Serial.print(" bpm | SpO2: "); Serial.print(data.spO2);
    Serial.print("% | Temp: "); Serial.print(data.bodyTemp);
    Serial.print("°C | Ambient: "); Serial.print(data.ambientTemp);
    Serial.print("°C | Humidity: "); Serial.print(data.humidity);
    Serial.print("% | Light: "); Serial.println(data.lightLevel);

    delay(1000);
}
