#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
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
#include "ui/menu.h"
#include "ui/ky040.h"

// Global Manager Instances
WiFiManager wifi;
FirebaseManager firebase;
UARTProtocol uart;
Display display;
Buzzer buzzer;
RotaryEncoder encoder;
MenuSystem* menu;

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
    delay(2000); // Give serial monitor time to connect
    Serial.println("=== SYNAPSE S3 BOOTING ===");

    // 1. Initialize I2C Bus FIRST (Crucial for Display and I2C Sensors)
    Serial.print("Initializing I2C Bus (SDA: "); Serial.print(I2C_SDA); 
    Serial.print(", SCL: "); Serial.print(I2C_SCL); Serial.println(")...");
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);
    Serial.println("I2C Bus Ready.");

    // 2. Initialize Buzzer (Audio feedback)
    Serial.print("Initializing Buzzer...");
    buzzer.begin();
    buzzer.playTone(TONE_CONFIRM);
    Serial.println("Done.");
    
    // 3. Initialize UI (Display requires I2C)
    Serial.print("Initializing Display...");
    display.begin();
    display.clear();
    display.print("Synapse S3");
    display.setCursor(0, 1);
    display.print("Booting Systems...");
    Serial.println("Done.");

    // 4. Initialize Communication
    Serial.print("Initializing WiFi...");
    wifi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Initiated.");

    // Wait for WiFi connection before initializing Firebase
    display.setCursor(0, 2);
    display.print("WiFi Connecting...");
    
    unsigned long wifiStartTime = millis();
    const unsigned long WIFI_TIMEOUT = 30000; // 30 seconds
    
    while (WiFi.status() != WL_CONNECTED && (millis() - wifiStartTime < WIFI_TIMEOUT)) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        display.setCursor(0, 2);
        display.print("WiFi: Connected  ");
        delay(500);
        
        // Only initialize Firebase if WiFi is connected
        Serial.print("Initializing Firebase...");
        firebase.begin(FIREBASE_API_KEY, FIREBASE_DATABASE_URL, FIREBASE_USER_EMAIL, FIREBASE_USER_PASSWORD);
        Serial.println("Initiated (auth in progress)");
        Serial.flush(); // Ensure message is sent before UART takes over pins
    } else {
        Serial.println("\nWiFi Connection Failed!");
        display.setCursor(0, 2);
        display.print("WiFi: FAILED    ");
        buzzer.playTone(TONE_ERROR);
        Serial.println("Skipping Firebase (No WiFi)");
        delay(2000);
    }

    // Serial.println("Initializing UART (Debug output will stop due to shared pins)...");
    // Serial.flush(); // Wait for message to send before pins are reconfigured
    // uart.begin();
    // Serial.println("Done."); // This won't be seen if pins are shared
    Serial.println("Initializing UART... SKIPPED (Debugging Mode)");

    // 5. Initialize Sensors
    Serial.print("Initializing HeartRate (MAX30102)...");
    heartRate.begin();
    Serial.println("Done.");

    Serial.print("Initializing Environmental (AM2302)...");
    environmental.begin();
    Serial.println("Done.");

    Serial.print("Initializing LightSensor...");
    lightSensor.begin();
    Serial.println("Done.");

    Serial.print("Initializing Ultrasonic...");
    ultrasonic.begin();
    Serial.println("Done.");

    Serial.print("Initializing LineSensor...");
    lineSensor.begin();
    Serial.println("Done.");

    Serial.print("Initializing MotionTracker (MPU6050)...");
    motion.begin();
    Serial.println("Done.");

    Serial.print("Initializing ColorSensor...");
    colorSensor.begin();
    Serial.println("Done.");

    Serial.print("Initializing LEDs...");
    leds.begin();
    Serial.println("Done.");

    Serial.print("Initializing Battery Monitor...");
    battery.begin();
    Serial.println("Done.");

    // 6. Instantiate Modes
    Serial.print("Setting up Modes & Menu...");
    monitoring = new MonitoringSystem(&heartRate, &environmental, &lightSensor, &display, &buzzer);
    assistant = new AssistantMode(&colorSensor, &ultrasonic, &uart, &display, &buzzer);
    lineFollower = new LineFollowing(&lineSensor, &uart, &display);
    obstacleAvoid = new ObstacleAvoidance(&ultrasonic, &motion, &uart, &display, &buzzer);
    autoLighting = new AutomaticLighting(&lightSensor, &leds);

    menu = new MenuSystem(&display, &buzzer, &encoder, autoLighting);

    autoLighting->begin();
    autoLighting->start();
    encoder.begin();
    menu->begin();
    Serial.println("Done.");

    Serial.println("=== SYSTEM READY ===");
    buzzer.doubleBeep();
}

void loop() {
    // Background Tasks
    wifi.update();
    
    // One-time Firebase authentication status check
    static bool firebaseAuthLogged = false;
    if (!firebaseAuthLogged && Firebase.ready()) {
        Serial.println("✓ Firebase Authentication Complete!");
        firebaseAuthLogged = true;
    }
    
    lightSensor.update();
    autoLighting->update();
    motion.update();
    buzzer.update();
    
    // UART Acknowledgment Check - SKIPPED FOR DEBUG
    // MotorCommand ackCmd;
    // uint8_t ackSpeed;
    // if (uart.receiveAcknowledgment(ackCmd, ackSpeed)) {
    //     Serial.println("ACK: Command received by Motor Controller");
    // }

    // Check for Firebase commands every loop for low latency
    static unsigned long lastFirebaseRx = 0;
    static FirebaseRxData rx;
    static int currentHR = 0, currentSpO2 = 0;
    static int usCenter = 0, usLeft = 0, usRear = 0, usRight = 0;
    static String currentColor = "UNKNOWN";
    static int currentCompartment = 255;

    if (millis() - lastFirebaseRx >= 100) { // Check every 100ms
        lastFirebaseRx = millis();
        if (firebase.receiveData(rx)) {
            // Actuator real-time control
            buzzer.controlFromFirebase(rx.buzzer01ring, rx.buzzer02ring, rx.buzzersound);
            leds.controlFromFirebase(rx.lightadj_left, rx.lightadj_right);
        }
    }

    // Real-time sensor monitoring (Mode independent)
    heartRate.monitorHeartRate(rx.heartrate_start, currentHR, currentSpO2);
    ultrasonic.monitorUltrasonic(rx.ultrasonic_start, usCenter, usLeft, usRear, usRight);
    currentColor = colorSensor.monitorColor(rx.colour_start);
    currentCompartment = lightSensor.monitorCompartment(rx.compartment_start);
    
    // Firebase Telemetry update (Keep at 2s)
    static unsigned long lastFirebaseUpdate = 0;
    if (millis() - lastFirebaseUpdate >= 2000) {
        lastFirebaseUpdate = millis();

        // Telemetry update using new methods and cached real-time values
        FirebaseTxData tx;
        motion.getMotionData(tx.acceleration, tx.angular);
        environmental.getEnvironmentData(tx.temp, tx.humidity);
        battery.getBatteryData(tx.battery, tx.voltage);
        
        tx.hr = currentHR;
        tx.sp02 = currentSpO2;
        tx.lightlevel = lightSensor.getPathLightLevel(); 
        
        tx.ultrasonic_center = usCenter;
        tx.ultrasonic_left = usLeft;
        tx.ultrasonic_rear = usRear;
        tx.ultrasonic_right = usRight;
        
        tx.colour = currentColor;
        tx.compartment = currentCompartment;
        
        firebase.sendData(tx);
    }

    // Update Menu and UI
    menu->update();
    menu->setEnvironmentalData(environmental.getTemperature(), environmental.getHumidity());
    menu->getBatteryLevel(battery.readPercentage());

    // Mode Execution Logic based on Menu Selection
    MenuState currentState = menu->getCurrentState();
    // COMMUNICATION FAILSAFE: SKIPPED FOR DEBUG
    // if (currentState != MAIN_MENU && currentState != SYSTEM_INFO && !uart.isConnected()) {
    //     static unsigned long lastCommWarning = 0;
    //     if (millis() - lastCommWarning > 5000) {
    //         Serial.println("⚠ WARNING: Communication with Motor Board LOST!");
    //         display.displayError("COMM LINK LOST");
    //         lastCommWarning = millis();
    //     }
    //     return; // Don't execute movement modes if disconnected
    // }

    static MenuState lastState = MAIN_MENU;
    static bool modeInitialized = false;

    // Handle Mode Transitions
    if (currentState != lastState) {
        // Stop the PREVIOUS mode
        switch (lastState) {
            case ASSISTANT_MODE:
                assistant->setFollowingMode(false);
                break;
            case MONITORING_MENU:
                monitoring->stopMonitoring();
                break;
            case LINE_FOLLOWING:
                lineFollower->stop();
                break;
            case OBSTACLE_AVOIDANCE_MODE:
                uart.sendMotorCommand(CMD_STOP, 0); // Ensure motors stop
                break;
            default: break;
        }
        
        lastState = currentState;
        modeInitialized = false; // Flag to Initialize the NEW mode
        
        // If returned to Main Menu, ensure display is refreshed 
        // (in case stop() methods wrote to screen)
        if (currentState == MAIN_MENU) {
             menu->updateDisplay();
        }
    }

    // Check for Splash Screen status
    // If splash is active (first 2 seconds of mode), SKIP mode execution.
    // The MenuSystem displays the "Running..." screen during this time.
    if (!menu->shouldShowSplash()) {
        
        // Initialize the NEW mode (Runs once after splash finishes)
        if (!modeInitialized && currentState != MAIN_MENU) {
             switch (currentState) {
                case ASSISTANT_MODE:
                    assistant->setFollowingMode(true);
                    // Ensure color sensor is ready
                    colorSensor.update(); 
                    break;
                case MONITORING_MENU:
                    // Calibrate sensors if needed before starting
                    // monitoring->calibrateSensors(); // Optional: Uncomment if auto-calibration desired
                    monitoring->startMonitoring();
                    // Force initial read
                    heartRate.update();
                    environmental.update();
                    break;
                case LINE_FOLLOWING:
                    lineFollower->start();
                    // Reset errors
                    lineSensor.update();
                    break;
                case OBSTACLE_AVOIDANCE_MODE:
                    // Ensure fresh sensor data for safety check immediately
                    ultrasonic.update();
                    motion.update();
                    break;
                default: break;
            }
            modeInitialized = true;
        }
    
        switch (currentState) {
            case MAIN_MENU:
                break;
                
            case ASSISTANT_MODE:
                assistant->update();
                break;
                
            case MONITORING_MENU:
                monitoring->update();
                break;
                
            case OBSTACLE_AVOIDANCE_MODE:
                obstacleAvoid->update();
                break;
                
            case LINE_FOLLOWING:
                if (lineFollower->isLineDetected()) {
                    lineFollower->update();
                }
                break;
                
            case SYSTEM_INFO:
                break;
                
            case AUTO_LIGHTING_SUBMENU:
                break;
                
            default:
                break;
        }
    }

    delay(10);
}
