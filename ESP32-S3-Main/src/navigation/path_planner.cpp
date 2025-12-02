#include "path_planner.h"
#include <math.h>

PathPlanner::PathPlanner() 
    : pathLength(0), currentSegment(0), pathValid(false), 
      totalDistance(0) {}

void PathPlanner::begin() {
    clearPath();
}

bool PathPlanner::planPath(PathPoint start, PathPoint goal) {
    clearPath();
    
    // Simple straight-line path for now
    // In a real implementation, this would use A* or similar
    addSegment(start, goal, true);
    
    pathValid = true;
    currentSegment = 0;
    
    // Calculate total distance
    totalDistance = 0;
    for (int i = 0; i < pathLength; i++) {
        totalDistance += currentPath[i].distance;
    }
    
    Serial.print("Path planned: ");
    Serial.print(pathLength);
    Serial.print(" segments, ");
    Serial.print(totalDistance);
    Serial.println(" mm total");
    
    return true;
}

void PathPlanner::addSegment(PathPoint start, PathPoint end, bool isLine) {
    if (pathLength >= 20) {
        Serial.println("Path buffer full!");
        return;
    }
    
    PathSegment seg;
    seg.start = start;
    seg.end = end;
    seg.distance = calculateDistance(start, end);
    seg.isLine = isLine;
    
    currentPath[pathLength++] = seg;
}

PathSegment PathPlanner::getCurrentSegment() {
    if (currentSegment < pathLength) {
        return currentPath[currentSegment];
    }
    return currentPath[pathLength - 1];
}

bool PathPlanner::isPathComplete() {
    return (currentSegment >= pathLength);
}

void PathPlanner::nextSegment() {
    if (currentSegment < pathLength) {
        currentSegment++;
        Serial.print("Moving to segment ");
        Serial.print(currentSegment);
        Serial.print("/");
        Serial.println(pathLength);
    }
}

float PathPlanner::getRemainingDistance() {
    float remaining = 0;
    for (int i = currentSegment; i < pathLength; i++) {
        remaining += currentPath[i].distance;
    }
    return remaining;
}

void PathPlanner::clearPath() {
    pathLength = 0;
    currentSegment = 0;
    pathValid = false;
    totalDistance = 0;
}

float PathPlanner::calculateDistance(PathPoint a, PathPoint b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrt(dx*dx + dy*dy);
}