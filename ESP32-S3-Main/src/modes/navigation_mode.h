#ifndef NAVIGATION_MODE_H
#define NAVIGATION_MODE_H

#include <Arduino.h>
#include "../sensors/line_follower.h"
#include "../sensors/ultrasonic.h"
#include "../sensors/motion_tracker.h"
#include "../communication/uart_protocol.h"
#include "../utils/pid_controller.h"

enum NavigationState {
    NAV_IDLE,
    NAV_FOLLOWING_LINE,
    NAV_AT_DESTINATION,
    NAV_PATH_LOST
};

struct Waypoint {
    int id;
    String location;
    bool reached;
};

class NavigationMode {
private:
    LineFollower* lineFollower;
    Ultrasonic* ultrasonic;
    MotionTracker* imu;
    UARTProtocol* uart;
    PIDController* linePID;
    NavigationState state;
    Waypoint currentWaypoint;
    float deviation;
    
public:
    NavigationMode(LineFollower* lf, Ultrasonic* us, MotionTracker* mt, UARTProtocol* u);
    void begin();
    void update();
    void loadDestination(String location);
    bool isDestinationReached();
    void followLine();
    void handlePathLost();
};

#endif