#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "actuators/ermc1604syg.h"
#include "actuators/sfm27.h"
#include "ky040.h"

enum MenuState {
    ASSISTANT_MODE_MENU,
    LINE_FOLLOWING_MENU,
    MONITORING_MENU,
    OBSTACLE_AVOIDANCE_MENU,
    SYSTEM_INFO_MENU,
};

struct RobotSettings {
    float followingDistance;  // meters (0.5 - 3.0)
    int speed;                // percentage (20-100)
    int brightness;           // percentage (0-100)
    int volume;               // 0-10
    int batteryLevel;
    bool wifiConnected;
};

class MenuSystem {
private:
    Display* display;
    Buzzer* buzzer;
    MenuState currentMenu;
    MenuState previousMenu;

    int currentSelection;
    int maxMenuItems;
    RobotSettings settings;
    bool inMenu;
    bool adjustingMode;
    int tempEncoderValue;
    
    // Menu display functions
    void displayMainMenuItems();
    void displayFollowModeMenu();
    void displayNavigationMenu();
    void displayMonitorMenu();
    void displaySettingsMenu();
    void displaySystemInfoMenu();
    
    // Menu selection handlers
    void handleMainMenuSelection();
    void handleFollowModeSelection();
    void handleNavigationSelection();
    void handleMonitorSelection();
    void handleSettingsSelection();
    
    // Value adjustment functions
    void startAdjustingDistance();
    void startAdjustingSpeed();
    void startAdjustingBrightness();
    void startAdjustingVolume();
    void updateAdjustingDisplay();
    
public:
    MenuSystem(Display* disp, Buzzer* bz);
    void begin();
    void enter();
    void exit();
    void navigate(int direction);
    void select();
    void adjustValue(int encoderChange);
    bool isActive();
    bool isAdjusting();
    void updateDisplay();
    
    // Settings getters
    float getFollowDistance();
    int getRobotSpeed();
    int getDisplayBrightness();
    int getBuzzerVolume();
    bool getAutoMode();
    void setBatteryLevel(int level);
    void setWifiStatus(bool connected);
    
    // Command getters for main loop
    bool shouldStartFollowing();
    bool shouldStartNavigation();
    bool shouldStartMonitoring();
    
    // Command flags (cleared after reading)
    bool startFollowingFlag;
    bool startNavigationFlag;
    bool startMonitoringFlag;

};

#endif