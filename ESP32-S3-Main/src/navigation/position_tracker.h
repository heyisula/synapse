#ifndef POSITION_TRACKER_H
#define POSITION_TRACKER_H

#include <Arduino.h>

struct RobotPosition {
    float x, y;
    float heading;
    float confidence;  // 0.0 to 1.0
};

class PositionTracker {
private:
    RobotPosition position;
    RobotPosition lastKnownGood;
    
    // Odometry integration
    float distanceTraveled;
    unsigned long lastUpdateTime;
    
    // Confidence decay
    float confidenceDecayRate;
    unsigned long lastLineDetection;
    
public:
    PositionTracker();
    void begin();
    void update(float deltaDistance, float deltaHeading);
    
    RobotPosition getPosition() { return position; }
    void setPosition(float x, float y, float heading);
    void resetPosition();
    
    void updateFromLine(int linePosition);
    void updateFromWaypoint(float x, float y);
    
    float getConfidence() { return position.confidence; }
    bool isPositionReliable() { return position.confidence > 0.6; }
    
private:
    void decayConfidence();
};

#endif