#include <Arduino.h>
#include "config/pins.h"
#include "config/constants.h"
#include "config/credentials.h"

// Communication
#include "communication/wifi_manager.h"
#include "communication/firebase_manager.h"
#include "communication/uart.h"

// Sensors & Actuators
#include "sensors/max30102.h"
#include "sensors/am2302.h"
#include "sensors/lm393.h"
#include "sensors/hcsr04.h"
#include "sensors/bfd1000.h"
#include "sensors/mpu6050.h"
#include "sensors/tcs3200.h"
#include "actuators/ermc1604syg.h"
#include "actuators/sfm27.h"
#include "actuators/le0066.h"
#include "utils/battery.h"

// Modes
#include "modes/monitoring.h"
#include "modes/assistant_mode.h"
#include "modes/line_following.h"
#include "modes/obstacle_avoidance.h"
#include "modes/automatic_lighting.h"

// Global Manager Instances
WiFiManager wifi;
FirebaseManager firebase;
UARTProtocol uart;
Display display;
Buzzer buzzer;

// Sensor Instances
HeartRateSensor heartRate;
Environmental environmental;
LightSensor lightSensor;
UltrasonicManager ultrasonic;
BFD1000 lineSensor;
MotionTracker motion;
ColorSensor colorSensor;
LEDArray leds;
Battery battery;

// Mode Instances
MonitoringSystem* monitoring;
AssistantMode* assistant;
LineFollowing* lineFollower;
ObstacleAvoidance* obstacleAvoid;
AutomaticLighting* autoLighting;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("=== SYNAPSE S3 BOOTING ===");

    // Initialize UI
    display.begin();
    display.clear();
    display.print("Synapse S3");
    display.setCursor(0, 1);
    display.print("Booting Systems...");

    buzzer.begin();
    buzzer.playTone(TONE_CONFIRM);

    // Initialize Communication
    wifi.begin(WIFI_SSID, WIFI_PASSWORD);
    firebase.begin(FIREBASE_API_KEY, FIREBASE_DATABASE_URL, FIREBASE_USER_EMAIL, FIREBASE_USER_PASSWORD);
    uart.begin();

    // Initialize Sensors
    heartRate.begin();
    environmental.begin();
    lightSensor.begin();
    ultrasonic.begin();
    lineSensor.begin();
    motion.begin();
    colorSensor.begin();
    leds.begin();
    battery.begin();

    // Instantiate Modes
    monitoring = new MonitoringSystem(&heartRate, &environmental, &lightSensor, &display, &buzzer);
    assistant = new AssistantMode(&colorSensor, &ultrasonic, &uart, &display, &buzzer);
    lineFollower = new LineFollowing(&lineSensor, &uart, &display);
    obstacleAvoid = new ObstacleAvoidance(&ultrasonic, &motion, &uart, &display, &buzzer);
    autoLighting = new AutomaticLighting(&lightSensor, &leds);

    autoLighting->begin();
    autoLighting->start();

    Serial.println("=== SYSTEM READY ===");
    buzzer.doubleBeep();
}

void loop() {
    // Background Tasks (Always Running)
    wifi.update();
    autoLighting->update();
    buzzer.update();
    
    // Firebase Comm
    static unsigned long lastFirebaseUpdate = 0;
    if (millis() - lastFirebaseUpdate >= 2000) {
        lastFirebaseUpdate = millis();
        
        FirebaseRxData rx;
        if (firebase.receiveData(rx)) {
            // Process commands from Firebase
            if (rx.heartrate_start) monitoring->startMonitoring();
            else if (monitoring->isMonitoring()) monitoring->stopMonitoring();

            assistant->setFollowingMode(rx.colour_start); // Hypothetical mapping
            
            buzzer.controlFromFirebase(rx.buzzer01ring, rx.buzzer02ring, rx.buzzersound);
        }

        // Telemetry update
        FirebaseTxData tx;
        tx.acceleration = motion.getAccelerationInt();
        tx.angular = motion.getAngularVelocityInt();
        tx.temp = environmental.getTemperature();
        tx.humidity = environmental.getHumidity();
        tx.hr = heartRate.getHeartRate();
        tx.sp02 = heartRate.getSpO2();
        tx.battery = battery.readPercentage();
        tx.voltage = battery.getVoltageInt();
        tx.lightlevel = lightSensor.getPathLightLevel(); 
        
        tx.ultrasonic_center = ultrasonic.getDistance(US_FRONT);
        tx.ultrasonic_left = ultrasonic.getDistance(US_LEFT);
        tx.ultrasonic_rear = ultrasonic.getDistance(US_BACK);
        tx.ultrasonic_right = ultrasonic.getDistance(US_RIGHT);
        
        tx.colour = colorSensor.monitorColor(false);
        tx.compartment = 0; // TBD
        
        firebase.sendData(tx);
    }

    // Mode Execution Logic
    // For now, let's run monitoring and obstacle avoidance as defaults
    monitoring->update();
    obstacleAvoid->update();

    if (lineFollower->isLineDetected()) {
        lineFollower->update();
    }
    
    if (assistant->isActive()) {
        assistant->update();
    }

    delay(10); // Minimal yield
}
