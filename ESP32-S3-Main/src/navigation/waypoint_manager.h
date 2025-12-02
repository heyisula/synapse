#ifndef WAYPOINT_MANAGER_H
#define WAYPOINT_MANAGER_H

#include <Arduino.h>

// RENAMED to avoid conflict
struct NavigationWaypoint {  // ✅ Changed from "Waypoint"
    String name;
    float x;
    float y;
    float heading;
    bool hasLineMarker;
};

class WaypointManager {
private:
    NavigationWaypoint waypoints[15];  // ✅ Updated
    int waypointCount;
    
    NavigationWaypoint currentWaypoint;  // ✅ Updated
    NavigationWaypoint targetWaypoint;   // ✅ Updated
    
public:
    WaypointManager();
    void begin();
    
    void addWaypoint(String name, float x, float y, float heading = 0, bool hasLine = true);
    bool setTarget(String name);
    NavigationWaypoint getTarget() { return targetWaypoint; }  // ✅ Updated
    NavigationWaypoint getCurrent() { return currentWaypoint; }  // ✅ Updated
    
    bool isNearWaypoint(float x, float y, float threshold = 200.0);
    NavigationWaypoint findNearestWaypoint(float x, float y);  // ✅ Updated
    
    void loadDefaultWaypoints();
    void listWaypoints();
    
private:
    int findWaypointIndex(String name);
};

#endif