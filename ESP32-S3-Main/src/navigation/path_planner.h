#ifndef PATH_PLANNER_H
#define PATH_PLANNER_H

#include <Arduino.h>

struct PathPoint {
    float x, y;
    float heading;
};

struct PathSegment {
    PathPoint start;
    PathPoint end;
    float distance;
    bool isLine;  // true for line following, false for direct
};

class PathPlanner {
private:
    PathSegment currentPath[20];
    int pathLength;
    int currentSegment;
    
    bool pathValid;
    float totalDistance;
    
public:
    PathPlanner();
    void begin();
    
    bool planPath(PathPoint start, PathPoint goal);
    PathSegment getCurrentSegment();
    bool isPathComplete();
    void nextSegment();
    
    float getRemainingDistance();
    int getSegmentCount() { return pathLength; }
    
    void clearPath();
    
private:
    void addSegment(PathPoint start, PathPoint end, bool isLine);
    float calculateDistance(PathPoint a, PathPoint b);
};

#endif