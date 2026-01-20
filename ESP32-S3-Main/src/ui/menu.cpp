#include "menu.h"
#include "ky040.h"
#include "actuators/ermc1604syg.h"
#include "actuators/sfm27.h"
#include "modes/automatic_lighting.h"
#include "../config/constants.h"

MenuSystem::MenuSystem(Display* disp, Buzzer* bz, RotaryEncoder* enc, AutomaticLighting* al) 
    : display(disp), buzzer(bz), encoder(enc), autoLighting(al) {
    currentMenu = MAIN_MENU;
    previousMenu = MAIN_MENU;
    currentSelection = 0;
    maxMenuItems = 6;
    inMenu = true;
    
    // Initialize settings
    settings.batteryLevel = 0;
    settings.temperature = 0.0f;
    settings.humidity = 0.0f;
    settings.currentSpeed = 0;
    settings.wifiConnected = false;
}

void MenuSystem::begin() {
    display->clear();
    updateDisplay();
}

void MenuSystem::update() {
    if (!encoder) return;

    encoder->update();

    int delta = encoder->getDelta();
    if (delta != 0) {
        currentSelection += delta;
        
        // Handle wrapping for arbitrary delta steps
        while (currentSelection < 0) {
            currentSelection += maxMenuItems;
        }
        while (currentSelection >= maxMenuItems) {
            currentSelection -= maxMenuItems;
        }
        
        updateDisplay();
    }

    if (encoder->isButtonReleased()) {
        select();
    }

    if (encoder->isLongPress()) {
        exit();
    }
}

void MenuSystem::select() {
    if (currentMenu == MAIN_MENU) {
        previousMenu = currentMenu;
        switch (currentSelection) {
            case 0: currentMenu = ASSISTANT_MODE; break;
            case 1: currentMenu = MONITORING_MENU; break;
            case 2: currentMenu = OBSTACLE_AVOIDANCE_MODE; break;
            case 3: currentMenu = LINE_FOLLOWING; break;
            case 4: currentMenu = SYSTEM_INFO; break;
            case 5: currentMenu = AUTO_LIGHTING_SUBMENU; 
                    currentSelection = 0; 
                    maxMenuItems = 2; 
                    break;
        }
    } else if (currentMenu == AUTO_LIGHTING_SUBMENU) {
        if (currentSelection == 0) {
            autoLighting->start();
        } else {
            autoLighting->stop();
        }
        currentMenu = MAIN_MENU;
        maxMenuItems = 6;
        currentSelection = 5; 
    }
    
    buzzer->playTone(TONE_CONFIRM);
    updateDisplay();
}

void MenuSystem::exit() {
    if (currentMenu != MAIN_MENU) {
        currentMenu = MAIN_MENU;
        maxMenuItems = 6;
        currentSelection = 0;
        buzzer->playTone(TONE_ERROR);
        updateDisplay();
    }
}

void MenuSystem::updateDisplay() {
    display->clear();
    switch (currentMenu) {
        case MAIN_MENU:
            displayMainMenuItems();
            break;
        case ASSISTANT_MODE:
            displayAssistantMode();
            break;
        case MONITORING_MENU:
            displayMonitoring();
            break;
        case OBSTACLE_AVOIDANCE_MODE:
            displayObstacleAvoidance();
            break;
        case LINE_FOLLOWING:
            displayLineFollowing();
            break;
        case SYSTEM_INFO:
            displaySystemInfo();
            break;
        case AUTO_LIGHTING_SUBMENU:
            displayAutomaticLighting();
            break;
        case AUTOMATIC_LIGHTING:
            // This is a transition state, display submenu instead if needed
            displayAutomaticLighting();
            break;
    }
}

void MenuSystem::displayMainMenuItems() {
    display->setCursor(0, 0);
    display->print("  SYNAPSE v1.0  ");
    
    const char* items[] = {
        "1.Assistant",
        "2.Monitoring",
        "3.Obs Avoid",
        "4.Line Follow",
        "5.System Info",
        "6.Auto Light"
    };

    int startItem = (currentSelection / 3) * 3;
    for (int i = 0; i < 3 && (startItem + i) < 6; i++) {
        display->setCursor(0, i + 1);
        if (currentSelection == (startItem + i)) {
            display->print("> ");
        } else {
            display->print("  ");
        }
        display->print(items[startItem + i]);
    }
}

void MenuSystem::displayAssistantMode() {
    display->setCursor(0, 0);
    display->print("MODE: ASSISTANT");
    display->setCursor(0, 1);
    display->print("Running...");
    display->setCursor(0, 3);
    display->print("Long press exit");
}

void MenuSystem::displayMonitoring() {
    display->setCursor(0, 0);
    display->print("MODE: MONITORING");
    display->setCursor(0, 1);
    display->print("Sensor Active");
    display->setCursor(0, 3);
    display->print("Long press exit");
}

void MenuSystem::displayObstacleAvoidance() {
    display->setCursor(0, 0);
    display->print("MODE: OBS AVOID");
    display->setCursor(0, 1);
    display->print("Scanning...");
    display->setCursor(0, 3);
    display->print("Long press exit");
}

void MenuSystem::displayLineFollowing() {
    display->setCursor(0, 0);
    display->print("MODE: LINEFOLLOW");
    display->setCursor(0, 1);
    display->print("Tracing...");
    display->setCursor(0, 3);
    display->print("Long press exit");
}

void MenuSystem::displaySystemInfo() {
    int activeModeNum = 0;
    // 1.assistant mode, 2.monitoring, 3.obstacle avoidance 4. line following
    switch (previousMenu) {
        case ASSISTANT_MODE: activeModeNum = 1; break;
        case MONITORING_MENU: activeModeNum = 2; break;
        case OBSTACLE_AVOIDANCE_MODE: activeModeNum = 3; break;
        case LINE_FOLLOWING: activeModeNum = 4; break;
        default: activeModeNum = 0; break;
    }
    
    display->displayStatus(settings.batteryLevel, settings.temperature, settings.humidity, activeModeNum);
}

void MenuSystem::displayAutomaticLighting() {
    display->setCursor(0, 0);
    display->print("-- AUTO LIGHT --");
    
    display->setCursor(0, 1);
    if (currentSelection == 0) display->print("> 1. ON");
    else display->print("  1. ON");
    
    display->setCursor(0, 2);
    if (currentSelection == 1) display->print("> 2. OFF");
    else display->print("  2. OFF");
    
    display->setCursor(0, 3);
    display->print("Press to Select");
}

void MenuSystem::setEnvironmentalData(float temp, float hum) {
    settings.temperature = temp;
    settings.humidity = hum;
}

void MenuSystem::getBatteryLevel(int level) {
    settings.batteryLevel = level;
}

void MenuSystem::getWifiStatus(bool connected) {
    settings.wifiConnected = connected;
}

void MenuSystem::getCurrentSpeed(int speed) {
    settings.currentSpeed = speed;
}

bool MenuSystem::isActive() {
    return inMenu;
}

void MenuSystem::enter() {
    inMenu = true;
    updateDisplay();
}
