#include <Arduino.h>
#include <Wire.h>
#include "config/pins.h"
#include "config/constants.h"
#include "config/thresholds.h"

// Sensors
#include "sensors/ultrasonic.h"
#include "sensors/line_follower.h"
#include "sensors/color_sensor.h"
#include "sensors/motion_tracker.h"
#include "sensors/heartrate_sensor.h"
#include "sensors/environmental.h"
#include "sensors/light_sensor.h"

// Actuators
#include "actuators/display.h"
#include "actuators/buzzer.h"
#include "actuators/led_array.h"

// UI
#include "ui/rotary_encoder.h"
#include "ui/menu_system.h"

// Communication
#include "communication/uart_protocol.h"
#include "communication/wifi_manager.h"


// Modes
#include "modes/following_mode.h"
#include "modes/navigation_mode.h"
#include "modes/monitoring_mode.h"
#include "modes/obstacle_mode.h"
#include "modes/environmental_mode.h"

// Navigation
#include "navigation/path_planner.h"
#include "navigation/waypoint_manager.h"
#include "navigation/position_tracker.h"

// Safety
#include "safety/collision_detection.h"
#include "safety/emergency_handler.h"
#include "safety/health_monitor.h"

// Utils
#include "utils/state_machine.h"
#include "utils/data_logger.h"

// ============================================================================
// GLOBAL OBJECTS - ALL COMPONENTS INITIALIZED
// ============================================================================

// Sensors
Ultrasonic ultrasonic;
LineFollower lineFollower;
ColorSensor colorSensor;
MotionTracker motionTracker;
HeartRateSensor heartRateSensor;
Environmental environmental(AM2302_DATA);
LightSensor lightSensor;

// Actuators
Display display;
Buzzer buzzer;
LEDArray ledArray;

// UI
RotaryEncoder rotaryEncoder;
MenuSystem menuSystem(&display, &buzzer);

// Communication
UARTProtocol uartProtocol;
WiFiManager wifiManager;           // ✓ NOW INITIALIZED

// Navigation
PathPlanner pathPlanner;           // ✓ NOW INITIALIZED
WaypointManager waypointManager;   // ✓ NOW INITIALIZED
PositionTracker positionTracker;   // ✓ NOW INITIALIZED

// Safety
CollisionDetector collisionDetector; // ✓ NOW INITIALIZED
EmergencyHandler emergencyHandler;    // ✓ NOW INITIALIZED
HealthMonitor healthMonitor;          // ✓ NOW INITIALIZED

// Utils
StateMachine stateMachine;
DataLogger dataLogger;             // ✓ NOW INITIALIZED

// Mode objects
FollowingMode* followingMode;
NavigationMode* navigationMode;
MonitoringMode* monitoringMode;
ObstacleMode* obstacleMode;
EnvironmentalMode* environmentalMode;

// ============================================================================
// TIMING VARIABLES
// ============================================================================
unsigned long lastSensorUpdate = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastEnvironmentCheck = 0;
unsigned long lastLightCheck = 0;           // ✓ FOR LDR SENSORS
unsigned long lastHealthCheck = 0;          // ✓ FOR HEALTH MONITOR
unsigned long lastDataLog = 0;              // ✓ FOR DATA LOGGER
unsigned long lastWiFiUpdate = 0;           // ✓ FOR WIFI MANAGER

// ============================================================================
// CONFIGURATION - CHANGE THESE FOR YOUR NETWORK
// ============================================================================
const char* WIFI_SSID = "YourWiFiSSID";         // ⚠️ CHANGE THIS
const char* WIFI_PASSWORD = "YourWiFiPassword"; // ⚠️ CHANGE THIS
const char* BT_DEVICE_NAME = "SYNAPSE-Robot";

// ============================================================================
// EMERGENCY CALLBACK
// ============================================================================
void onEmergencyTriggered(EmergencyType type) {
    Serial.println("⚠️  EMERGENCY CALLBACK TRIGGERED");
    uartProtocol.sendEmergencyStop();
    ledArray.setBoth(LED_BLINK_FAST);
    buzzer.emergencyAlarm();
    
    // Log emergency event
    dataLogger.logError("Emergency: " + String((int)type));
    
    // Send WiFi alert if connected
    if (wifiManager.isConnected()) {
        wifiManager.sendAlert("Emergency Stop Activated", 4);
    }
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(1000);
    
    Serial.println("=================================");
    Serial.println("SYNAPSE Healthcare Robot");
    Serial.println("ESP32-S3 Main Controller");
    Serial.println("Firmware Version 2.0 - Full Featured");
    Serial.println("=================================");
    
    // Initialize I2C (MUST be before sensors that use I2C)
    Serial.println("\n[1/11] Initializing I2C Bus...");
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);
    
    // Initialize Display First (for status messages)
    Serial.println("[2/11] Initializing Display...");
    display.begin();
    display.clear();
    display.setCursor(0, 0);
    display.print(" SYNAPSE Robot ");
    display.setCursor(0, 1);
    display.print(" Initializing  ");
    display.setCursor(0, 2);
    display.print(" Please Wait...");
    
    // Initialize Actuators
    Serial.println("[3/11] Initializing Actuators...");
    buzzer.begin();
    ledArray.begin();
    buzzer.singleBeep();
    ledArray.setBoth(LED_SOLID);
    
    // Initialize Sensors
    Serial.println("[4/11] Initializing Sensors...");
    ultrasonic.begin();
    lineFollower.begin();
    colorSensor.begin();
    lightSensor.begin();  // ✓ LDR SENSORS INITIALIZED
    
    if(motionTracker.begin()) {
        Serial.println("  ✓ MPU6050 initialized");
        healthMonitor.reportSensorUpdate(0);
    } else {
        Serial.println("  ✗ MPU6050 init failed!");
        dataLogger.logError("MPU6050 initialization failed");
    }
    
    if(heartRateSensor.begin()) {
        Serial.println("  ✓ MAX30102 initialized");
        healthMonitor.reportSensorUpdate(1);
    } else {
        Serial.println("  ✗ MAX30102 init failed!");
        dataLogger.logError("MAX30102 initialization failed");
    }
    
    if(environmental.begin()) {
        Serial.println("  ✓ AM2303 (DHT22) initialized");
        healthMonitor.reportSensorUpdate(2);
    } else {
        Serial.println("  ✗ AM2303 init failed!");
        dataLogger.logError("AM2303 initialization failed");
    }
    
    // Initialize UI
    Serial.println("[5/11] Initializing UI...");
    rotaryEncoder.begin();
    menuSystem.begin();
    buzzer.setVolume(menuSystem.getBuzzerVolume());
    
    // Initialize Communication
    Serial.println("[6/11] Initializing Communication...");
    uartProtocol.begin();
    
    // Initialize WiFi (optional - can fail without stopping system)
    Serial.print("[7/11] Initializing WiFi... ");
    wifiManager.begin(WIFI_SSID, WIFI_PASSWORD);
    if (wifiManager.isConnected()) {
        Serial.println("✓ Connected");
        menuSystem.setWifiStatus(true);
        dataLogger.logEvent("WIFI", "Connected to " + String(WIFI_SSID));
    } else {
        Serial.println("✗ Not connected (continuing anyway)");
        menuSystem.setWifiStatus(false);
    }
    
    
    
    // Initialize Navigation
    Serial.println("[9/11] Initializing Navigation...");
    pathPlanner.begin();
    waypointManager.begin();
    positionTracker.begin();
    waypointManager.listWaypoints();  // Print available waypoints
    
    // Initialize Safety Systems
    Serial.println("[10/11] Initializing Safety Systems...");
    collisionDetector.begin();
    emergencyHandler.begin();
    healthMonitor.begin();
    
    // Set emergency callback
    emergencyHandler.setOnEmergencyStart(onEmergencyTriggered);
    
    // Initialize State Machine & Data Logger
    Serial.println("[11/11] Initializing State Machine & Logger...");
    stateMachine.begin();
    dataLogger.begin();
    
    // Initialize Modes
    Serial.println("\nInitializing Operation Modes...");
    followingMode = new FollowingMode(&ultrasonic, &colorSensor, &uartProtocol);
    navigationMode = new NavigationMode(&lineFollower, &ultrasonic, &motionTracker, &uartProtocol);
    monitoringMode = new MonitoringMode(&heartRateSensor, &buzzer, &display);
    obstacleMode = new ObstacleMode(&ultrasonic, &uartProtocol, &buzzer);
    environmentalMode = new EnvironmentalMode(&environmental, &lightSensor, &display, &buzzer);
    
    // Startup sequence complete
    buzzer.doubleBeep();
    ledArray.setBoth(LED_OFF);
    delay(200);
    ledArray.setBoth(LED_SOLID);
    delay(200);
    ledArray.setBoth(LED_OFF);
    
    Serial.println("\n=================================");
    Serial.println("✓ All Systems Initialized");
    Serial.println("✓ System Ready for Operation");
    Serial.println("=================================\n");
    
    // Log system startup
    dataLogger.logEvent("SYSTEM", "Startup complete");
    dataLogger.logEvent("HEALTH", healthMonitor.getHealthReport());
    
    // Display ready message
    display.clear();
    display.setCursor(0, 0);
    display.print("System Ready");
    display.setCursor(0, 1);
    display.print("WiFi: " + String(wifiManager.isConnected() ? "Yes" : "No"));
    display.setCursor(0, 2);
    display.print("Long press for");
    display.setCursor(0, 3);
    display.print("menu");
    delay(2000);
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
    unsigned long currentTime = millis();
    
    // ========================================================================
    // 1. UPDATE EMERGENCY HANDLER (HIGHEST PRIORITY)
    // ========================================================================
    emergencyHandler.update();
    
    if (emergencyHandler.isEmergencyActive()) {
        // Emergency mode - stop everything
        stateMachine.setState(STATE_EMERGENCY);
        uartProtocol.sendEmergencyStop();
        ledArray.setBoth(LED_BLINK_FAST);
        
        display.clear();
        display.setCursor(0, 0);
        display.print("*** EMERGENCY ***");
        display.setCursor(0, 1);
        display.print(emergencyHandler.getEmergencyDescription());
        
        delay(100);
        return; // Don't process anything else
    }
    
    // ========================================================================
    // 2. UPDATE ROTARY ENCODER
    // ========================================================================
    rotaryEncoder.update();
    
    // ========================================================================
    // 3. MENU HANDLING
    // ========================================================================
    if(!menuSystem.isActive()) {
        // Not in menu - check for long press to enter menu
        if(rotaryEncoder.isLongPress()) {
            menuSystem.enter();
            buzzer.doubleBeep();
            Serial.println("Menu entered");
            dataLogger.logEvent("UI", "Menu opened");
        }
        
        // Check for menu command flags
        if(menuSystem.shouldStartFollowing()) {
            Serial.println("Starting Following Mode from menu");
            stateMachine.setState(STATE_FOLLOWING);
            followingMode->begin();
            followingMode->setTargetDistance(menuSystem.getFollowDistance() * 100);
            dataLogger.logEvent("MODE", "Following mode started");
        }
        
        if(menuSystem.shouldStartNavigation()) {
            Serial.print("Starting Navigation Mode - Destination: ");
            Serial.println(menuSystem.getNavigationDestination());
            stateMachine.setState(STATE_NAVIGATION);
            navigationMode->begin();
            
            const char* destinations[] = {
                "Doctor's Office",
                "Laboratory", 
                "Pharmacy",
                "Nursing Station",
                "Return to Base"
            };
            navigationMode->loadDestination(destinations[menuSystem.getNavigationDestination()]);
            dataLogger.logEvent("MODE", "Navigation to " + String(destinations[menuSystem.getNavigationDestination()]));
        }
        
        if(menuSystem.shouldStartMonitoring()) {
            Serial.println("Starting Monitoring Mode from menu");
            stateMachine.setState(STATE_MONITORING);
            monitoringMode->begin();
            dataLogger.logEvent("MODE", "Monitoring mode started");
        }
    } else {
        // In menu mode
        int encoderPos = rotaryEncoder.getPosition();
        if(encoderPos != 0) {
            menuSystem.navigate(encoderPos > 0 ? 1 : -1);
            rotaryEncoder.resetPosition();
        }
        
        if(rotaryEncoder.isButtonReleased()) {
            menuSystem.select();
        }
        
        if(rotaryEncoder.isLongPress() && !menuSystem.isAdjusting()) {
            menuSystem.exit();
            buzzer.tripleBeep();
            Serial.println("Menu exited");
            dataLogger.logEvent("UI", "Menu closed");
        }
    }
    
    // ========================================================================
    // 4. SENSOR UPDATE CYCLE
    // ========================================================================
    if(currentTime - lastSensorUpdate >= SENSOR_READ_INTERVAL) {
        ultrasonic.update();
        lineFollower.update();
        colorSensor.update();
        motionTracker.update();
        heartRateSensor.update();
        environmental.update();
        lightSensor.update();  // ✓ LDR SENSORS NOW UPDATED
        
        // Report sensor updates to health monitor
        healthMonitor.reportSensorUpdate(0); // MPU6050
        healthMonitor.reportSensorUpdate(1); // MAX30102
        healthMonitor.reportSensorUpdate(2); // AM2303
        
        lastSensorUpdate = currentTime;
    }
    
    // ========================================================================
    // 5. COLLISION DETECTION (USES ULTRASONIC DATA)
    // ========================================================================
    collisionDetector.update(
        ultrasonic.getDistance(US_FRONT),
        ultrasonic.getDistance(US_BACK),
        ultrasonic.getDistance(US_LEFT),
        ultrasonic.getDistance(US_RIGHT)
    );
    
    if (collisionDetector.isCollisionDetected()) {
        CollisionStatus status = collisionDetector.getStatus();
        
        if (status.severity == SEVERITY_IMMINENT) {
            emergencyHandler.triggerEmergency(
                EMERGENCY_COLLISION, 
                "Imminent collision: " + String(status.distance) + "cm"
            );
        } else if (status.severity == SEVERITY_CRITICAL) {
            buzzer.playTone(TONE_WARNING);
            ledArray.setBoth(LED_BLINK_FAST);
        }
    }
    
    // ========================================================================
    // 6. OBSTACLE AVOIDANCE (ALWAYS ACTIVE)
    // ========================================================================
    obstacleMode->update();
    
    if(!obstacleMode->isPathClear()) {
        if (stateMachine.getState() != STATE_OBSTACLE_AVOIDANCE) {
            stateMachine.setState(STATE_OBSTACLE_AVOIDANCE);
            dataLogger.logEvent("OBSTACLE", "Obstacle detected, avoiding");
        }
        ledArray.setBoth(LED_BLINK_FAST);
    } else {
        // Path is clear
        if (stateMachine.getState() == STATE_OBSTACLE_AVOIDANCE) {
            // Was avoiding, now clear - return to previous state
            stateMachine.setState(stateMachine.getPreviousState());
            dataLogger.logEvent("OBSTACLE", "Path clear, resuming");
        }
        ledArray.setBoth(LED_OFF);
        
        // ====================================================================
        // 7. MAIN STATE MACHINE (ONLY WHEN NOT IN MENU OR AVOIDING OBSTACLE)
        // ====================================================================
        if(!menuSystem.isActive()) {
            switch(stateMachine.getState()) {
                case STATE_IDLE:
                    uartProtocol.sendMotorCommand(CMD_STOP, 0);
                    ledArray.setPattern(true, LED_PULSE);
                    break;
                    
                case STATE_FOLLOWING: {  // ✓ Added curly braces
                    followingMode->update();
                    ledArray.setPattern(true, LED_SOLID);
                    followingMode->setTargetDistance(menuSystem.getFollowDistance() * 100);
                    
                    // Log following data periodically
                    if (currentTime - lastDataLog > 5000) {
                        dataLogger.logEvent("FOLLOW", 
                            "Distance: " + String(ultrasonic.getDistance(US_FRONT)) + 
                            " Priority: " + String(followingMode->getPriority()));
                    }
                    break;
                }  // ✓ Close curly braces
                    
                case STATE_NAVIGATION: {  // ✓ Added curly braces
                    navigationMode->update();
                    ledArray.setPattern(false, LED_SOLID);
                    
                    if(navigationMode->isDestinationReached()) {
                        stateMachine.setState(STATE_IDLE);
                        buzzer.doubleBeep();
                        
                        display.clear();
                        display.setCursor(0, 1);
                        display.print("Destination");
                        display.setCursor(0, 2);
                        display.print("Reached!");
                        
                        dataLogger.logEvent("NAV", "Destination reached");
                        
                        if (wifiManager.isConnected()) {
                            wifiManager.sendAlert("Destination reached", 1);
                        }
                        
                        delay(2000);
                    }
                    break;
                }  // ✓ Close curly braces
                    
                case STATE_MONITORING: {  // ✓ Added curly braces
                    monitoringMode->update();
                    ledArray.setBoth(LED_PULSE);
                    
                    // Log vital signs
                    HealthData vitals = monitoringMode->getCurrentData();
                    if (vitals.valid && currentTime - lastDataLog > 5000) {
                        dataLogger.logVitalSigns(vitals.heartRate, vitals.spO2);
                        
                        // Send critical alerts via WiFi
                        if (wifiManager.isConnected()) {
                            if (heartRateSensor.isTachycardia()) {
                                wifiManager.sendAlert("Tachycardia detected: " + String(vitals.heartRate), 3);
                            }
                            if (heartRateSensor.isLowOxygen()) {
                                wifiManager.sendAlert("Low oxygen: " + String(vitals.spO2) + "%", 4);
                            }
                        }
                    }
                    break;
                }  // ✓ Close curly braces
                    
                case STATE_ENVIRONMENTAL_CHECK:
                    environmentalMode->update();
                    break;
                    
                case STATE_EMERGENCY:
                    // Already handled above
                    break;
                    
                case STATE_ERROR:
                    uartProtocol.sendMotorCommand(CMD_STOP, 0);
                    buzzer.playTone(TONE_ERROR);
                    display.displayError("System Error");
                    dataLogger.logError("System error state");
                    break;
            }
        }
    }
    
    // ========================================================================
    // 8. LIGHT SENSOR MONITORING (LDR SENSORS)
    // ========================================================================
    if (currentTime - lastLightCheck >= 1000) {  // Check every second
        // Check path visibility
        if (lightSensor.getPathDarkness() >= PATH_LIGHT_THRESHOLD) {
            Serial.println("Warning: Low light on path");
            buzzer.singleBeep();
            dataLogger.logEvent("LIGHT", "Low path visibility");
            
            if (wifiManager.isConnected()) {
                wifiManager.sendAlert("Low light conditions", 2);
            }
        }
        static bool compartmentWasOpen = false;
        bool compartmentIsOpen = lightSensor.isCompartmentOpen();
        // Check compartment security
        if (compartmentIsOpen && !compartmentWasOpen) {
            Serial.println("Compartment opened");
            dataLogger.logEvent("SECURITY", "Compartment accessed");
        }
        
        lastLightCheck = currentTime;
    }
    
    // ========================================================================
    // 9. ENVIRONMENTAL MONITORING
    // ========================================================================
    if (currentTime - lastEnvironmentCheck >= 5000) {  // Every 5 seconds
        float temp = environmental.getTemperature();
        float humidity = environmental.getHumidity();
        
        if (!environmental.isOptimalTemperature() || !environmental.isOptimalHumidity()) {
            Serial.print("⚠️  Environment: ");
            Serial.print(temp);
            Serial.print("°C, ");
            Serial.print(humidity);
            Serial.println("% RH");
            
            dataLogger.logEvent("ENV", 
                "T:" + String(temp, 1) + "C H:" + String(humidity, 1) + "%");
        }
        
        lastEnvironmentCheck = currentTime;
    }
    
    // ========================================================================
    // 10. HEALTH MONITORING
    // ========================================================================
    if (currentTime - lastHealthCheck >= 10000) {  // Every 10 seconds
        healthMonitor.update();
        
        if (!healthMonitor.isSystemHealthy()) {
            Serial.println("⚠️  System health degraded");
            Serial.print(healthMonitor.getHealthReport());
            
            dataLogger.logError("System health check failed");
            
            if (wifiManager.isConnected()) {
                wifiManager.sendAlert("System health degraded", 3);
            }
        }
        
        // Report battery voltage (example - would read from ADC in real implementation)
        float batteryVoltage = 11.8;  // TODO: Read from actual voltage divider
        healthMonitor.reportBatteryVoltage(batteryVoltage);
        
        if (batteryVoltage < BATTERY_LOW_VOLTAGE) {
            menuSystem.setBatteryLevel((int)((batteryVoltage - 10.0) / 2.6 * 100));
            buzzer.playTone(TONE_WARNING);
            
            if (batteryVoltage < BATTERY_CRITICAL_VOLTAGE) {
                emergencyHandler.triggerEmergency(
                    EMERGENCY_BATTERY_CRITICAL,
                    "Battery critical: " + String(batteryVoltage, 1) + "V"
                );
            }
        }
        
        lastHealthCheck = currentTime;
    }
    
    // ========================================================================
    // 11. DISPLAY UPDATE
    // ========================================================================
    if(currentTime - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        if(menuSystem.isActive()) {
            // Menu is handling display
        } else {
            // Show normal status
            String stateStr;
            switch(stateMachine.getState()) {
                case STATE_IDLE: stateStr = "Idle"; break;
                case STATE_FOLLOWING: stateStr = "Following"; break;
                case STATE_NAVIGATION: stateStr = "Navigation"; break;
                case STATE_MONITORING: stateStr = "Monitoring"; break;
                case STATE_OBSTACLE_AVOIDANCE: stateStr = "Obstacle!"; break;
                case STATE_EMERGENCY: stateStr = "EMERGENCY"; break;
                default: stateStr = "Unknown"; break;
            }
            
            int batteryLevel = 87;  // TODO: Calculate from actual battery voltage
            menuSystem.setBatteryLevel(batteryLevel);
            
            display.displayStatus(
                stateStr,
                batteryLevel,
                ultrasonic.getDistance(US_FRONT) / 100.0,
                heartRateSensor.getHeartRate(),
                heartRateSensor.getSpO2()
            );
        }
        
        lastDisplayUpdate = currentTime;
    }
    
    // ========================================================================
    // 12. LED ARRAY UPDATE
    // ========================================================================
    ledArray.update();
    
    // ========================================================================
    // 13. BUZZER VOLUME CONTROL
    // ========================================================================
    buzzer.setVolume(menuSystem.getBuzzerVolume());
    
    // ========================================================================
    // 14. WIFI MANAGER UPDATE
    // ========================================================================
    if (currentTime - lastWiFiUpdate >= 5000) {  // Every 5 seconds
        wifiManager.update();
        menuSystem.setWifiStatus(wifiManager.isConnected());
        lastWiFiUpdate = currentTime;
    }
    
    // ========================================================================
    // 16. DATA LOGGING
    // ========================================================================
    if (currentTime - lastDataLog >= 5000) {  // Log every 5 seconds
        // Log position if in navigation mode
        if (stateMachine.getState() == STATE_NAVIGATION) {
            RobotPosition pos = positionTracker.getPosition();
            dataLogger.logPosition(pos.x, pos.y, pos.heading);
        }
        
        lastDataLog = currentTime;
    }
    
    // ========================================================================
    // 17. HEARTBEAT (SYSTEM STATUS)
    // ========================================================================
    if(currentTime - lastHeartbeat >= HEARTBEAT_INTERVAL) {
        Serial.print("State: ");
        Serial.print(stateMachine.getState());
        Serial.print(" | Menu: ");
        Serial.print(menuSystem.isActive() ? "Active" : "Inactive");
        Serial.print(" | Front: ");
        Serial.print(ultrasonic.getDistance(US_FRONT));
        Serial.print(" cm | HR: ");
        Serial.print(heartRateSensor.getHeartRate());
        Serial.print(" | SpO2: ");
        Serial.print(heartRateSensor.getSpO2());
        Serial.print("% | WiFi: ");
        Serial.print(wifiManager.isConnected() ? "Yes" : "No");
    
        Serial.print(" | Speed: ");
        Serial.print(menuSystem.getRobotSpeed());
        Serial.println("%");
        
        // Report UART response to health monitor
        healthMonitor.reportUARTResponse();
        
        lastHeartbeat = currentTime;
    }
    
    // Small delay to prevent watchdog issues
    delay(10);
}  