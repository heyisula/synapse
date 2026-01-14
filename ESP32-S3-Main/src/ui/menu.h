#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

#include "actuators/ermc1604syg.h"
#include "actuators/sfm27.h" 
#include "ky040.h"

#include "modes/automatic_lighting.h"
#include "modes/assistant_mode.h"
#include "modes/line_following.h"
#include "modes/monitoring.h"
#include "modes/obstacle_avoidance.h"

enum MenuState {
    MAIN_MENU,
    ASSISTANT_MODE,
    LINE_FOLLOWING,
    MONITORING_MENU,
    OBSTACLE_AVOIDANCE_MODE,
    SYSTEM_INFO,
    AUTOMATIC_LIGHTING
};

struct SystemInfo {
    int batteryLevel;
    int currentSpeed;
    bool wifiConnected;
};

class MenuSystem {
private:
    Display* display;
    Buzzer* buzzer;
    MenuState currentMenu;
    MenuState previousMenu;
    SystemInfo settings;

    int currentSelection;
    int maxMenuItems;
    bool inMenu;
    
    void displayMainMenuItems();
    void displayAssistantMode();
    void displayLineFollowing();
    void displayMonitoring();
    void displayObstacleAvoidance();
    void displaySystemInfo();
    void displayAutomaticLighting();
    
public:
    MenuSystem(Display* disp, Buzzer* bz);
    void begin();
    void enter();
    void exit();
    void select();
    bool isActive();
    void updateDisplay();
    
    void getBatteryLevel(int level);
    void getWifiStatus(bool connected);
    void getCurrentSpeed(int speed);

};

#endif