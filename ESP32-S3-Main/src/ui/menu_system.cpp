#include "menu_system.h"

MenuSystem::MenuSystem(Display* disp, Buzzer* bz) {
    display = disp;
    buzzer = bz;
    currentMenu = MAIN_MENU;
    previousMenu = MAIN_MENU;
    currentSelection = 0;
    maxMenuItems = 5;
    inMenu = false;
    adjustingMode = false;
    tempEncoderValue = 0;
    
    // Initialize settings with defaults
    settings.followingDistance = 1.5;
    settings.speed = 70;
    settings.brightness = 80;
    settings.volume = 7;
    settings.autoMode = false;
    settings.batteryLevel = 100;
    settings.wifiConnected = false;
    
    // Initialize command flags
    startFollowingFlag = false;
    startNavigationFlag = false;
    startMonitoringFlag = false;
    selectedDestination = DEST_DOCTORS_OFFICE;
}

void MenuSystem::begin() {
    settings.followingDistance = 1.5;
    settings.speed = 70;
    settings.brightness = 80;
    settings.volume = 7;
    settings.autoMode = false;
}

void MenuSystem::enter() {
    inMenu = true;
    currentMenu = MAIN_MENU;
    currentSelection = 0;
    maxMenuItems = 5;
    updateDisplay();
}

void MenuSystem::exit() {
    inMenu = false;
    adjustingMode = false;
}

void MenuSystem::navigate(int direction) {
    if(adjustingMode) {
        // In adjusting mode, use this for value changes
        adjustValue(direction);
        return;
    }
    
    currentSelection += direction;
    
    // Wrap around
    if(currentSelection < 0) currentSelection = maxMenuItems - 1;
    if(currentSelection >= maxMenuItems) currentSelection = 0;
    
    buzzer->singleBeep();
    updateDisplay();
}

void MenuSystem::select() {
    buzzer->doubleBeep();
    
    if(adjustingMode) {
        // Confirm adjustment
        adjustingMode = false;
        updateDisplay();
        return;
    }
    
    switch(currentMenu) {
        case MAIN_MENU:
            handleMainMenuSelection();
            break;
        case FOLLOW_MODE_MENU:
            handleFollowModeSelection();
            break;
        case NAVIGATION_MENU:
            handleNavigationSelection();
            break;
        case MONITOR_MENU:
            handleMonitorSelection();
            break;
        case SETTINGS_MENU:
            handleSettingsSelection();
            break;
        case SYSTEM_INFO_MENU:
            currentMenu = MAIN_MENU;
            currentSelection = 0;
            maxMenuItems = 5;
            updateDisplay();
            break;
        default:
            break;
    }
}

void MenuSystem::adjustValue(int encoderChange) {
    if(!adjustingMode) return;
    
    tempEncoderValue += encoderChange;
    
    switch(previousMenu) {
        case FOLLOW_MODE_MENU:
            if(currentSelection == 1) {
                settings.followingDistance += encoderChange * 0.1;
                settings.followingDistance = constrain(settings.followingDistance, 0.5, 3.0);
            } else if(currentSelection == 2) {
                settings.speed += encoderChange * 5;
                settings.speed = constrain(settings.speed, 20, 100);
            }
            break;
            
        case SETTINGS_MENU:
            if(currentSelection == 0) {
                settings.speed += encoderChange * 5;
                settings.speed = constrain(settings.speed, 20, 100);
            } else if(currentSelection == 1) {
                settings.brightness += encoderChange * 5;
                settings.brightness = constrain(settings.brightness, 0, 100);
            } else if(currentSelection == 2) {
                settings.volume += encoderChange;
                settings.volume = constrain(settings.volume, 0, 10);
                buzzer->setVolume(settings.volume);
                buzzer->singleBeep(); // Test volume
            }
            break;
        default:
            break;
    }
    
    updateAdjustingDisplay();
}

bool MenuSystem::isActive() {
    return inMenu;
}

bool MenuSystem::isAdjusting() {
    return adjustingMode;
}

void MenuSystem::updateDisplay() {
    display->clear();
    
    switch(currentMenu) {
        case MAIN_MENU:
            displayMainMenuItems();
            break;
        case FOLLOW_MODE_MENU:
            displayFollowModeMenu();
            break;
        case NAVIGATION_MENU:
            displayNavigationMenu();
            break;
        case MONITOR_MENU:
            displayMonitorMenu();
            break;
        case SETTINGS_MENU:
            displaySettingsMenu();
            break;
        case SYSTEM_INFO_MENU:
            displaySystemInfoMenu();
            break;
        default:
            break;
    }
}

void MenuSystem::displayMainMenuItems() {
    display->setCursor(0, 0);
    display->print("=== MAIN MENU ==");
    
    const char* items[] = {
        "Follow Mode",
        "Navigation Mode",
        "Monitor Mode",
        "Settings",
        "System Info"
    };
    
    // Display 3 items at a time with scrolling
    for(int i = 0; i < 3 && i < maxMenuItems; i++) {
        int itemIndex = currentSelection + i;
        if(itemIndex >= maxMenuItems) itemIndex = itemIndex - maxMenuItems;
        
        display->setCursor(0, i + 1);
        if(i == 0) {
            display->print("> ");
        } else {
            display->print("  ");
        }
        display->print(items[itemIndex]);
    }
}

void MenuSystem::displayFollowModeMenu() {
    display->setCursor(0, 0);
    display->print("== FOLLOW MODE =");
    
    display->setCursor(0, 1);
    if(currentSelection == 0) display->print("> ");
    else display->print("  ");
    display->print("Start Following");
    
    display->setCursor(0, 2);
    if(currentSelection == 1) display->print("> ");
    else display->print("  ");
    display->print("Dist:");
    display->print(settings.followingDistance, 1);
    display->print("m");
    
    display->setCursor(0, 3);
    if(currentSelection == 2) display->print("> ");
    else display->print("  ");
    display->print("Speed:");
    display->print(settings.speed);
    display->print("%");
}

void MenuSystem::displayNavigationMenu() {
    display->setCursor(0, 0);
    display->print("== NAVIGATION ==");
    
    const char* destinations[] = {
        "Doctor's Office",
        "Laboratory",
        "Pharmacy",
        "Nursing Station",
        "Return to Base",
        "Back"
    };
    
    // Display 3 items with scrolling
    for(int i = 0; i < 3 && i < maxMenuItems; i++) {
        int itemIndex = currentSelection + i;
        if(itemIndex >= maxMenuItems) itemIndex = itemIndex - maxMenuItems;
        
        display->setCursor(0, i + 1);
        if(i == 0) {
            display->print("> ");
        } else {
            display->print("  ");
        }
        
        // Truncate long names
        String dest = destinations[itemIndex];
        if(dest.length() > 14) dest = dest.substring(0, 14);
        display->print(dest);
    }
}

void MenuSystem::displayMonitorMenu() {
    display->setCursor(0, 0);
    display->print("== MONITOR MODE");
    
    display->setCursor(0, 1);
    if(currentSelection == 0) display->print("> ");
    else display->print("  ");
    display->print("Start Monitor");
    
    display->setCursor(0, 2);
    if(currentSelection == 1) display->print("> ");
    else display->print("  ");
    display->print("Alert Settings");
    
    display->setCursor(0, 3);
    if(currentSelection == 2) display->print("> ");
    else display->print("  ");
    display->print("Back");
}

void MenuSystem::displaySettingsMenu() {
    display->setCursor(0, 0);
    display->print("== SETTINGS ====");
    
    // Build menu items
    String items[5];
    items[0] = "Speed:" + String(settings.speed) + "%";
    items[1] = "Bright:" + String(settings.brightness) + "%";
    items[2] = "Volume:" + String(settings.volume);
    items[3] = "Auto:" + String(settings.autoMode ? "ON" : "OFF");
    items[4] = "Back";
    
    // Display 3 items with scrolling
    for(int i = 0; i < 3 && i < maxMenuItems; i++) {
        int itemIndex = currentSelection + i;
        if(itemIndex >= maxMenuItems) itemIndex = itemIndex - maxMenuItems;
        
        display->setCursor(0, i + 1);
        if(i == 0) {
            display->print("> ");
        } else {
            display->print("  ");
        }
        display->print(items[itemIndex]);
    }
}

void MenuSystem::displaySystemInfoMenu() {
    display->setCursor(0, 0);
    display->print("== SYSTEM INFO =");
    
    display->setCursor(0, 1);
    display->print("Battery: ");
    display->print(settings.batteryLevel);
    display->print("%");
    
    display->setCursor(0, 2);
    display->print("WiFi: ");
    display->print(settings.wifiConnected ? "Connected" : "No WiFi");
    
    display->setCursor(0, 3);
    display->print("Press to exit");
}

void MenuSystem::handleMainMenuSelection() {
    switch(currentSelection) {
        case 0: // Follow Mode
            currentMenu = FOLLOW_MODE_MENU;
            currentSelection = 0;
            maxMenuItems = 3;
            break;
            
        case 1: // Navigation Mode
            currentMenu = NAVIGATION_MENU;
            currentSelection = 0;
            maxMenuItems = 6;
            break;
            
        case 2: // Monitor Mode
            currentMenu = MONITOR_MENU;
            currentSelection = 0;
            maxMenuItems = 3;
            break;
            
        case 3: // Settings
            currentMenu = SETTINGS_MENU;
            currentSelection = 0;
            maxMenuItems = 5;
            break;
            
        case 4: // System Info
            currentMenu = SYSTEM_INFO_MENU;
            break;
    }
    updateDisplay();
}

void MenuSystem::handleFollowModeSelection() {
    switch(currentSelection) {
        case 0: // Start Following
            startFollowingFlag = true;
            display->clear();
            display->setCursor(0, 1);
            display->print("Following Mode");
            display->setCursor(0, 2);
            display->print("Starting...");
            buzzer->tripleBeep();
            delay(1500);
            exit();
            break;
            
        case 1: // Adjust Distance
            previousMenu = FOLLOW_MODE_MENU;
            adjustingMode = true;
            startAdjustingDistance();
            break;
            
        case 2: // Adjust Speed
            previousMenu = FOLLOW_MODE_MENU;
            adjustingMode = true;
            startAdjustingSpeed();
            break;
    }
}

void MenuSystem::handleNavigationSelection() {
    if(currentSelection == 5) {
        // Back button
        currentMenu = MAIN_MENU;
        currentSelection = 0;
        maxMenuItems = 5;
        updateDisplay();
    } else {
        // Start navigation
        const char* destinations[] = {
            "Doctor's Office",
            "Laboratory",
            "Pharmacy",
            "Nursing Station",
            "Return to Base"
        };
        
        selectedDestination = (NavigationDestination)currentSelection;
        startNavigationFlag = true;
        
        display->clear();
        display->setCursor(0, 0);
        display->print("Navigating to:");
        display->setCursor(0, 1);
        
        String dest = destinations[currentSelection];
        if(dest.length() > 16) dest = dest.substring(0, 16);
        display->print(dest);
        
        display->setCursor(0, 2);
        display->print("Please wait...");
        
        buzzer->tripleBeep();
        delay(2000);
        exit();
    }
}

void MenuSystem::handleMonitorSelection() {
    switch(currentSelection) {
        case 0: // Start Monitor
            startMonitoringFlag = true;
            display->clear();
            display->setCursor(0, 1);
            display->print("Monitor Mode");
            display->setCursor(0, 2);
            display->print("Starting...");
            buzzer->tripleBeep();
            delay(1500);
            exit();
            break;
            
        case 1: // Alert Settings
            display->clear();
            display->setCursor(0, 0);
            display->print("Alert Settings");
            display->setCursor(0, 1);
            display->print("Not implemented");
            buzzer->playTone(TONE_WARNING);
            delay(1500);
            updateDisplay();
            break;
            
        case 2: // Back
            currentMenu = MAIN_MENU;
            currentSelection = 0;
            maxMenuItems = 5;
            updateDisplay();
            break;
    }
}

void MenuSystem::handleSettingsSelection() {
    switch(currentSelection) {
        case 0: // Speed
            previousMenu = SETTINGS_MENU;
            adjustingMode = true;
            startAdjustingSpeed();
            break;
            
        case 1: // Brightness
            previousMenu = SETTINGS_MENU;
            adjustingMode = true;
            startAdjustingBrightness();
            break;
            
        case 2: // Volume
            previousMenu = SETTINGS_MENU;
            adjustingMode = true;
            startAdjustingVolume();
            break;
            
        case 3: // Auto Mode
            settings.autoMode = !settings.autoMode;
            buzzer->doubleBeep();
            updateDisplay();
            break;
            
        case 4: // Back
            currentMenu = MAIN_MENU;
            currentSelection = 0;
            maxMenuItems = 5;
            updateDisplay();
            break;
    }
}

void MenuSystem::startAdjustingDistance() {
    display->clear();
    display->setCursor(0, 0);
    display->print("Following Dist.");
    display->setCursor(0, 2);
    display->print("Rotate to adjust");
    display->setCursor(0, 3);
    display->print("Press to confirm");
    updateAdjustingDisplay();
}

void MenuSystem::startAdjustingSpeed() {
    display->clear();
    display->setCursor(0, 0);
    display->print("Robot Speed");
    display->setCursor(0, 2);
    display->print("Rotate to adjust");
    display->setCursor(0, 3);
    display->print("Press to confirm");
    updateAdjustingDisplay();
}

void MenuSystem::startAdjustingBrightness() {
    display->clear();
    display->setCursor(0, 0);
    display->print("LCD Brightness");
    display->setCursor(0, 2);
    display->print("Rotate to adjust");
    display->setCursor(0, 3);
    display->print("Press to confirm");
    updateAdjustingDisplay();
}

void MenuSystem::startAdjustingVolume() {
    display->clear();
    display->setCursor(0, 0);
    display->print("Buzzer Volume");
    display->setCursor(0, 2);
    display->print("Rotate to adjust");
    display->setCursor(0, 3);
    display->print("Press to confirm");
    updateAdjustingDisplay();
}

void MenuSystem::updateAdjustingDisplay() {
    display->setCursor(0, 1);
    display->print("                "); // Clear line
    display->setCursor(0, 1);
    
    if(previousMenu == FOLLOW_MODE_MENU && currentSelection == 1) {
        display->print("Distance: ");
        display->print(settings.followingDistance, 1);
        display->print("m  ");
    } else if((previousMenu == FOLLOW_MODE_MENU && currentSelection == 2) || 
              (previousMenu == SETTINGS_MENU && currentSelection == 0)) {
        display->print("Speed: ");
        display->print(settings.speed);
        display->print("%   ");
    } else if(previousMenu == SETTINGS_MENU && currentSelection == 1) {
        display->print("Brightness: ");
        display->print(settings.brightness);
        display->print("%   ");
    } else if(previousMenu == SETTINGS_MENU && currentSelection == 2) {
        display->print("Volume: ");
        display->print(settings.volume);
        display->print("/10   ");
    }
}

// Getters
float MenuSystem::getFollowDistance() {
    return settings.followingDistance;
}

int MenuSystem::getRobotSpeed() {
    return settings.speed;
}

int MenuSystem::getDisplayBrightness() {
    return settings.brightness;
}

int MenuSystem::getBuzzerVolume() {
    return settings.volume;
}

bool MenuSystem::getAutoMode() {
    return settings.autoMode;
}

void MenuSystem::setBatteryLevel(int level) {
    settings.batteryLevel = constrain(level, 0, 100);
}

void MenuSystem::setWifiStatus(bool connected) {
    settings.wifiConnected = connected;
}

bool MenuSystem::shouldStartFollowing() {
    bool result = startFollowingFlag;
    startFollowingFlag = false;
    return result;
}

bool MenuSystem::shouldStartNavigation() {
    bool result = startNavigationFlag;
    startNavigationFlag = false;
    return result;
}

bool MenuSystem::shouldStartMonitoring() {
    bool result = startMonitoringFlag;
    startMonitoringFlag = false;
    return result;
}

NavigationDestination MenuSystem::getNavigationDestination() {
    return selectedDestination;
}