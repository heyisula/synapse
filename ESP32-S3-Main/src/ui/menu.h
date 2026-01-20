#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

// Forward declarations
class Display;
class Buzzer;
class RotaryEncoder;
class AutomaticLighting;

enum MenuState {
    MAIN_MENU,
    ASSISTANT_MODE,
    MONITORING_MENU,
    OBSTACLE_AVOIDANCE_MODE,
    LINE_FOLLOWING,
    SYSTEM_INFO,
    AUTOMATIC_LIGHTING,
    AUTO_LIGHTING_SUBMENU
};

struct SystemInfo {
    int batteryLevel;
    float temperature;
    float humidity;
    int currentSpeed;
    bool wifiConnected;
};

class MenuSystem {
private:
    Display* display;
    Buzzer* buzzer;
    RotaryEncoder* encoder;
    AutomaticLighting* autoLighting;
    MenuState currentMenu;
    MenuState previousMenu;
    SystemInfo settings;

    int currentSelection;
    int maxMenuItems;
    bool inMenu;
    
    // Splash screen Transition
    unsigned long splashStartTime;
    bool isSplashActive;
    
    void displayMainMenuItems();
    void displayAssistantMode();
    void displayLineFollowing();
    void displayMonitoring();
    void displayObstacleAvoidance();
    void displaySystemInfo();
    void displayAutomaticLighting();
    
public:
    MenuSystem(Display* disp, Buzzer* bz, RotaryEncoder* enc, AutomaticLighting* al);
    void begin();
    void update();
    MenuState getCurrentState() { return currentMenu; }
    void setEnvironmentalData(float temp, float hum);
    void enter();
    void exit();
    void select();
    bool isActive();
    void updateDisplay();
    bool shouldShowSplash();
    
    void getBatteryLevel(int level);
    void getWifiStatus(bool connected);
    void getCurrentSpeed(int speed);

};

#endif