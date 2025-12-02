#include "waypoint_manager.h"

WaypointManager::WaypointManager() 
    : waypointCount(0) {}

void WaypointManager::begin() {
    loadDefaultWaypoints();
}

void WaypointManager::addWaypoint(String name, float x, float y, 
                                  float heading, bool hasLine) {
    if (waypointCount >= 15) {
        Serial.println("Waypoint buffer full!");
        return;
    }
    
    waypoints[waypointCount].name = name;
    waypoints[waypointCount].x = x;
    waypoints[waypointCount].y = y;
    waypoints[waypointCount].heading = heading;
    waypoints[waypointCount].hasLineMarker = hasLine;
    
    waypointCount++;
}

bool WaypointManager::setTarget(String name) {
    int index = findWaypointIndex(name);
    if (index >= 0) {
        targetWaypoint = waypoints[index];
        Serial.print("Target set to: ");
        Serial.println(targetWaypoint.name);
        return true;
    }
    
    Serial.print("Waypoint not found: ");
    Serial.println(name);
    return false;
}

int WaypointManager::findWaypointIndex(String name) {
    for (int i = 0; i < waypointCount; i++) {
        if (waypoints[i].name.equalsIgnoreCase(name)) {
            return i;
        }
    }
    return -1;
}

bool WaypointManager::isNearWaypoint(float x, float y, float threshold) {
    for (int i = 0; i < waypointCount; i++) {
        float dx = waypoints[i].x - x;
        float dy = waypoints[i].y - y;
        float distance = sqrt(dx*dx + dy*dy);
        
        if (distance < threshold) {
            currentWaypoint = waypoints[i];
            return true;
        }
    }
    return false;
}

NavigationWaypoint WaypointManager::findNearestWaypoint(float x, float y) {
    float minDist = 999999;
    int nearest = 0;
    
    for (int i = 0; i < waypointCount; i++) {
        float dx = waypoints[i].x - x;
        float dy = waypoints[i].y - y;
        float distance = sqrt(dx*dx + dy*dy);
        
        if (distance < minDist) {
            minDist = distance;
            nearest = i;
        }
    }
    
    return waypoints[nearest];
}

void WaypointManager::loadDefaultWaypoints() {
    // Hospital waypoints (coordinates in mm)
    addWaypoint("Base Station", 0, 0, 0, true);
    addWaypoint("Doctor's Office", 3000, 0, 0, true);
    addWaypoint("Laboratory", 6000, 2000, 90, true);
    addWaypoint("Pharmacy", 6000, 5000, 180, true);
    addWaypoint("Nursing Station", 3000, 5000, 270, true);
    addWaypoint("Patient Room 1", 1000, 2000, 0, true);
    addWaypoint("Patient Room 2", 1000, 3000, 0, true);
    addWaypoint("Emergency", 8000, 2500, 90, false);
    
    Serial.print("Loaded ");
    Serial.print(waypointCount);
    Serial.println(" waypoints");
}

void WaypointManager::listWaypoints() {
    Serial.println("\n=== Available Waypoints ===");
    for (int i = 0; i < waypointCount; i++) {
        Serial.print(i + 1);
        Serial.print(". ");
        Serial.print(waypoints[i].name);
        Serial.print(" (");
        Serial.print(waypoints[i].x);
        Serial.print(", ");
        Serial.print(waypoints[i].y);
        Serial.println(")");
    }
    Serial.println();
}