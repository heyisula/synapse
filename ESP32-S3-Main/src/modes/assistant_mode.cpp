#include "assistant_mode.h"
#include "config/constants.h"
#include "config/thresholds.h"

AssistantMode::AssistantMode(ColorSensor* cs, UltrasonicManager* us, 
                                        UARTProtocol* uart_proto, Display* disp, Buzzer* buzz) {
    colorSensor = cs;
    ultrasonicMgr = us;
    uart = uart_proto;
    display = disp;
    buzzer = buzz;
    
    currentState = ASSISTANT_STATE_IDLE;
    identifiedStaff = STAFF_UNKNOWN;
    humanDetected = false;
    isSystemActive = false;
    isFollowingMode = false;
    colorSensorCalibrated = false;
    
    targetDistance = TARGET_FOLLOW_DISTANCE;
    distanceTolerance = DISTANCE_TOLERANCE;
    
    stateStartTime = 0;
    lastDisplayUpdate = 0;
    trackingLostTime = 0;
    
    centerDistance = 0;
    leftDistance = 0;
    rearDistance = 0;
    rightDistance = 0;
}

void AssistantMode::begin() {
    Serial.println("==============================================");
    Serial.println("    STAFF IDENTIFICATION SYSTEM INITIALIZED   ");
    Serial.println("==============================================");
    
    display->clear();
    display->setCursor(0, 0);
    display->print("  SYNAPSE Robot ");
    display->setCursor(0, 1);
    display->print("Staff ID System");
    display->setCursor(0, 2);
    display->print("   Initializing  ");
    display->setCursor(0, 3);
    display->print("  Please Wait... ");
    
    delay(2000);
    
    buzzer->playTone(TONE_CONFIRM);
    Serial.println("System ready for operation");
}

void AssistantMode::setFollowingMode(bool enable) {
    if (enable && !isFollowingMode) {
        isFollowingMode = true;
        isSystemActive = true;
        currentState = ASSISTANT_STATE_WAITING_CARD;
        stateStartTime = millis();
        
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║  HUMAN FOLLOWING MODE ACTIVATED        ║");
        Serial.println("╚════════════════════════════════════════╝");
        
        display->clear();
        display->setCursor(0, 0);
        display->print("Following Mode");
        display->setCursor(0, 1);
        display->print("   ACTIVATED    ");
        
        buzzer->playTone(TONE_CONFIRM);
        delay(1000);
        
    } else if (!enable && isFollowingMode) {
        isFollowingMode = false;
        isSystemActive = false;
        currentState = ASSISTANT_STATE_IDLE;
        stopRobot();
        
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║  HUMAN FOLLOWING MODE DEACTIVATED      ║");
        Serial.println("╚════════════════════════════════════════╝");
        
        display->clear();
        display->setCursor(0, 0);
        display->print("Following Mode");
        display->setCursor(0, 1);
        display->print("  DEACTIVATED   ");
        
        buzzer->playTone(TONE_WARNING);
    }
}

void AssistantMode::update() {
    if (!isSystemActive) return;
    
    unsigned long now = millis();
    buzzer->update(); // Handle pattern beeps in background
    if (now - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        displayCurrentState();
        lastDisplayUpdate = now;
    }
    
    switch (currentState) {
        case ASSISTANT_STATE_WAITING_CARD:
            waitForCardPunch();
            break;
            
        case ASSISTANT_STATE_IDENTIFYING:
            identifyStaff();
            break;
            
        case ASSISTANT_STATE_IDENTIFIED_DELAY:
            if (millis() - stateStartTime > 2000) {
                currentState = ASSISTANT_STATE_WAITING_DISTANCE;
                stateStartTime = millis();
            }
            break;
            
        case ASSISTANT_STATE_WAITING_DISTANCE:
            waitForProperDistance();
            break;
            
        case ASSISTANT_STATE_DISTANCE_CONFIRM_DELAY:
            if (millis() - stateStartTime > 2000) {
                currentState = ASSISTANT_STATE_FOLLOWING;
                stateStartTime = millis();
                humanDetected = true;
            }
            break;
            
        case ASSISTANT_STATE_FOLLOWING:
            followHuman();
            break;
            
        case ASSISTANT_STATE_TRACKING_LOST:
            handleTrackingLost();
            break;
            
        case ASSISTANT_STATE_TRACKING_LOST_DELAY:
            if (millis() - stateStartTime > 3000) {
                currentState = ASSISTANT_STATE_WAITING_DISTANCE;
                stateStartTime = millis();
            }
            break;
            
        case ASSISTANT_STATE_ERROR:
            stopRobot();
            break;
            
        default:
            currentState = ASSISTANT_STATE_IDLE;
            break;
    }
}

void AssistantMode::waitForCardPunch() {
    display->clear();
    display->setCursor(0, 0);
    display->print("Color Sensor");
    display->setCursor(0, 1);
    display->print("    STARTED     ");
    display->setCursor(0, 2);
    display->print("                ");
    display->setCursor(0, 3);
    display->print("Punch Your Card!");
    
    Serial.println("┌──────────────────────────────────────┐");
    Serial.println("│ Waiting for Staff Card...           │");
    Serial.println("│ Please punch your ID card            │");
    Serial.println("└──────────────────────────────────────┘");
    
    if (!colorSensorCalibrated) {
        colorSensor->calibrate();
        colorSensorCalibrated = true;
        delay(500);
    }
    
    String detectedColor = colorSensor->monitorColor(true);
    
    if (detectedColor != "UNKNOWN") {
        currentState = ASSISTANT_STATE_IDENTIFYING;
        stateStartTime = millis();
        buzzer->playTone(TONE_CONFIRM);
        Serial.println("Card detected! Moving to identification...");
    }
}

void AssistantMode::identifyStaff() {
    colorSensor->update();
    ColorType detectedColor = colorSensor->getColorType();
    identifiedStaff = colorToStaffType(detectedColor);
    
    if (identifiedStaff != STAFF_UNKNOWN) {
        String staffName = staffTypeToString(identifiedStaff);
        
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.print("║  IDENTIFIED: ");
        Serial.print(staffName);
        for (int i = staffName.length(); i < 24; i++) Serial.print(" ");
        Serial.println("║");
        Serial.println("╚════════════════════════════════════════╝");
        
        display->clear();
        display->setCursor(0, 0);
        display->print("  IDENTIFIED:   ");
        display->setCursor(0, 1);
        
        if (identifiedStaff == STAFF_DOCTOR) {
            display->print("     DOCTOR     ");
        } else if (identifiedStaff == STAFF_NURSE) {
            display->print("     NURSE      ");
        } else if (identifiedStaff == STAFF_MEDICAL_STUDENT) {
            display->print(" Medical Student");
        }
        
        buzzer->doubleBeep();
        currentState = ASSISTANT_STATE_IDENTIFIED_DELAY;
        stateStartTime = millis();
        
    } else {
        Serial.println("Unknown card detected, waiting...");
        
        if (millis() - stateStartTime > CARD_IDENTIFICATION_TIMEOUT) {
            currentState = ASSISTANT_STATE_WAITING_CARD;
            display->displayError("Unknown Card!");
            buzzer->playTone(TONE_ERROR);
            delay(2000);
        }
    }
}

void AssistantMode::waitForProperDistance() {
    display->clear();
    display->setCursor(0, 0);
    display->print("Please stand at");
    display->setCursor(0, 1);
    display->print("  30cm distance ");
    display->setCursor(0, 2);
    display->print("                ");
    display->setCursor(0, 3);
    display->print("Measuring...");
    
    Serial.println("┌──────────────────────────────────────┐");
    Serial.println("│ Please stand 30cm away from robot   │");
    Serial.println("└──────────────────────────────────────┘");
    
    bool ultrasonicActive = ultrasonicMgr->monitorUltrasonic(true, 
        centerDistance, leftDistance, rearDistance, rightDistance);
    
    if (ultrasonicActive) {
        if (centerDistance >= (TARGET_FOLLOW_DISTANCE - DISTANCE_TOLERANCE) && 
            centerDistance <= (TARGET_FOLLOW_DISTANCE + DISTANCE_TOLERANCE)) {
            
            Serial.println("\n✓ Correct distance achieved!");
            Serial.println("✓ All systems stable");
            Serial.println("✓ Starting human following...");
            
            display->clear();
            display->setCursor(0, 0);
            display->print("Systems Stable!");
            display->setCursor(0, 1);
            display->print("Distance: OK");
            display->setCursor(0, 2);
            display->print("                ");
            display->setCursor(0, 3);
            display->print("Starting Follow ");
            
            buzzer->singleBeep();
            
            currentState = ASSISTANT_STATE_DISTANCE_CONFIRM_DELAY;
            stateStartTime = millis();
            humanDetected = true;
            
        } else {
            display->setCursor(0, 3);
            display->print("Now: ");
            display->print(centerDistance);
            display->print("cm       ");
        }
    }
}

void AssistantMode::followHuman() {
    bool ultrasonicActive = ultrasonicMgr->monitorUltrasonic(true, 
        centerDistance, leftDistance, rearDistance, rightDistance);
    
    if (!ultrasonicActive) {
        currentState = ASSISTANT_STATE_ERROR;
        display->displayError("Sensor Error!");
        return;
    }
    
    if (centerDistance > MAX_FOLLOW_DISTANCE) {
        if (trackingLostTime == 0) {
            trackingLostTime = millis();
        } else if (millis() - trackingLostTime > TRACKING_LOST_TIMEOUT) {
            currentState = ASSISTANT_STATE_TRACKING_LOST;
            trackingLostTime = 0;
            return;
        }
    } else {
        trackingLostTime = 0;
        humanDetected = true;
    }
    
    display->clear();
    display->setCursor(0, 0);
    display->print("FOLLOWING: ");
    display->print(staffTypeToString(identifiedStaff).substring(0, 4));
    display->setCursor(0, 1);
    display->print("Dist: ");
    display->print(centerDistance);
    display->print("cm      ");
    display->setCursor(0, 2);
    display->print("L:");
    display->print(leftDistance);
    display->print(" R:");
    display->print(rightDistance);
    display->print("    ");
    display->setCursor(0, 3);
    display->print("Status: Active  ");
    
    sendFollowingCommand();
}

void AssistantMode::handleTrackingLost() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║  WARNING: TRACKING LOST!               ║");
    Serial.println("╚════════════════════════════════════════╝");
    
    display->clear();
    display->setCursor(0, 0);
    display->print(" TRACKING LOST! ");
    display->setCursor(0, 1);
    display->print("                ");
    display->setCursor(0, 2);
    display->print("Please return to");
    display->setCursor(0, 3);
    display->print("visible range   ");
    
    stopRobot();
    buzzer->tripleBeep();
    
    currentState = ASSISTANT_STATE_TRACKING_LOST_DELAY;
    stateStartTime = millis();
}

void AssistantMode::sendFollowingCommand() {
    int distanceError = centerDistance - targetDistance;
    
    int lateralError = leftDistance - rightDistance;
    
    if (centerDistance < MIN_FOLLOW_DISTANCE) {
        uart->sendMotorCommand(CMD_EMERGENCY_STOP, 0);
        buzzer->playTone(TONE_WARNING);
        Serial.println("⚠ Too close! Emergency stop!");
        return;
    }
    
    //1: Rotation
    if (abs(lateralError) > 15) {
        if (lateralError > 0) {
            uart->sendMotorCommand(CMD_RIGHT, 30);
            Serial.println("→ Adjusting right");
        } else {
            uart->sendMotorCommand(CMD_LEFT, 30);
            Serial.println("← Adjusting left");
        }
    } 
    //2: Distance
    else if (abs(distanceError) > distanceTolerance) {
        
        if (distanceError > distanceTolerance) {
            uint8_t speed = constrain(map(distanceError, distanceTolerance, 50, 30, 80), 30, 80);
            uart->sendMotorCommand(CMD_FORWARD, speed);
            Serial.print("↑ Moving forward, speed: ");
            Serial.println(speed);
            
        } else if (distanceError < -distanceTolerance) {
            uint8_t speed = 40;
            uart->sendMotorCommand(CMD_BACKWARD, speed);
            Serial.print("↓ Moving backward, speed: ");
            Serial.println(speed);
        }
    }
    //3: Stop
    else {
        uart->sendMotorCommand(CMD_STOP, 0);
        Serial.println("→ Maintaining distance");
    }
}

void AssistantMode::stopRobot() {
    uart->sendMotorCommand(CMD_STOP, 0);
    Serial.println("■ Robot stopped");
}

StaffType AssistantMode::colorToStaffType(ColorType color) {
    switch (color) {
        case COLOR_BLUE:
            return STAFF_DOCTOR;
        case COLOR_RED:
            return STAFF_MEDICAL_STUDENT;
        case COLOR_GREEN:
            return STAFF_NURSE;
        
        default:
            return STAFF_UNKNOWN;
    }
}

String AssistantMode::staffTypeToString(StaffType staff) {
    switch (staff) {
        case STAFF_DOCTOR:
            return "Doctor";
        case STAFF_MEDICAL_STUDENT:
            return "Medical Student";
        case STAFF_NURSE:
            return "Nurse";
        
        default:
            return "Unknown";
    }
}

void AssistantMode::displayCurrentState() {
    display->clear();
    display->setCursor(0, 0);
    display->print("Mode: Following");
    display->setCursor(0, 1);
    display->print("Staff: ");
    display->print(staffTypeToString(identifiedStaff).substring(0, 4));
    display->setCursor(0, 2);
    display->print("Dist: ");
    display->print(centerDistance);
    display->print("cm      ");
    display->setCursor(0, 3);
    display->print("State: ");
    
    switch (currentState) {
        case ASSISTANT_STATE_WAITING_CARD:
            display->print("Waiting Card ");
            break;
        case ASSISTANT_STATE_IDENTIFYING:
            display->print("Identifying  ");
            break;
        case ASSISTANT_STATE_IDENTIFIED_DELAY:
            display->print("Identified   ");
            break;
        case ASSISTANT_STATE_WAITING_DISTANCE:
            display->print("Waiting Dist ");
            break;
        case ASSISTANT_STATE_DISTANCE_CONFIRM_DELAY:
            display->print("Dist Confirm ");
            break;
        case ASSISTANT_STATE_FOLLOWING:
            display->print("Following    ");
            break;
        case ASSISTANT_STATE_TRACKING_LOST:
            display->print("Tracking Lost");
            break;
        case ASSISTANT_STATE_TRACKING_LOST_DELAY:
            display->print("Lost Delay   ");
            break;
        case ASSISTANT_STATE_ERROR:
            display->print("ERROR        ");
            break;
        default:
            display->print("Idle         ");
            break;
    }
}